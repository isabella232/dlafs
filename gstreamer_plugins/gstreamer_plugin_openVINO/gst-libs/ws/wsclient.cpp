/*
 *Copyright (C) 2018 Intel Corporation
 *
 *SPDX-License-Identifier: LGPL-2.1-only
 *
 *This library is free software; you can redistribute it and/or modify it under the terms
 * of the GNU Lesser General Public License as published by the Free Software Foundation;
 * version 2.1.
 *
 *This library is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along with this library;
 * if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
  * Author: River,Li
  * Email: river.li@intel.com
  * Date: 2018.10
  */

 #include <glib.h>
#include <gst/gst.h>
#include "wsclient.h"
#include <uWS/uWS.h>
#include <string.h>

using namespace std;
#include <thread>
#include <string>
#include <sstream>

#ifdef __cplusplus
 extern "C" {
#endif
 
 struct _wsclient{
     uWS::WebSocket<uWS::CLIENT> *client;
     int id;
     char *data;
     int data_len;
     GAsyncQueue *message_queue;
 };
 typedef struct _wsclient WsClient;
 
#define DEFAULT_WSS_URI "wss://localhost:8123/binaryEchoWithSize?id=3"

static void item_free_func(gpointer data)
{
       MessageItem *item = ( MessageItem *)data;
      if(item && (item->len>0)){
            g_free(item->data);
       }
       return;
}

 WsClientHandle wsclient_setup(const char *serverUri, int client_id)
 {
     WsClient *wsclient = (WsClient *)g_new0 (WsClient, 1);
     if(!wsclient) {
         g_print("Failed to calloc WsClient!\n");
         return 0;
     }
     wsclient->client = NULL;
     wsclient->id = client_id;
     wsclient->data = NULL;
     wsclient->data_len = 0;
     wsclient->message_queue = g_async_queue_new_full (item_free_func);

     std::thread t([&wsclient, serverUri, client_id]{
         uWS::Hub hub;
 
         hub.onConnection([wsclient, serverUri, client_id](uWS::WebSocket<uWS::CLIENT> *ws, uWS::HttpRequest req) {
             wsclient->client = ws;
             std::stringstream ss;
             std::string s;
             ss << client_id;
             ss >> s;
             std::string msg = std::string("pipe_id=") + s;
             ws->send(msg.c_str(), msg.size(), uWS::OpCode::TEXT);
             g_print("Success to connect with %s!\n", serverUri);
         });

        // If receive message, put it to queue
        hub.onMessage([wsclient ](uWS::WebSocket<uWS::CLIENT> *ws, char *message, size_t length, uWS::OpCode opCode){
            g_print("Receive receive message = %s!\n", message);
            if(opCode ==  uWS::OpCode::TEXT) {
                MessageItem *item = g_new0(MessageItem, 1);
                item->data = g_new0(char, length);
                item->len = length;
                g_memmove(item->data, message, length);
                g_async_queue_push ( wsclient->message_queue, item);
             }else{
                g_print("Receive message that not need to be process: size = %ld\n", length);
             }
         });

         hub.onError([](void *user) {
             g_print("FAILURE: Connection failed! Timeout?\n");
             exit(-1);
         });
 
         hub.onDisconnection([](uWS::WebSocket<uWS::CLIENT> *ws, int code, char *message, size_t length) {
             g_print("CLIENT CLOSE: %d: %s\n", code, message);
         });
 
         if(!serverUri){
             hub.connect(DEFAULT_WSS_URI, nullptr);
             g_print("Try to default websocket server: %s\n", DEFAULT_WSS_URI);
         }else{
             hub.connect(serverUri, nullptr);
             g_print("Try to connect websocket server: %s\n", serverUri);
         }
         hub.run();
     });
     t.detach();

    int times=1000;
     while(!wsclient->client  && times-->0)
         g_usleep(1000);

     return (WsClientHandle)wsclient;
 }
 
 void wsclient_send_data(WsClientHandle handle, char *data, int len)
 {
     WsClient *wsclient = (WsClient *)handle;
 
     if(!handle) {
         g_print("Invalid WsClientHandle!!!\n");
         return;
     }
     if(!wsclient->data) {
        wsclient->data_len = len + 4;
        wsclient->data = g_new0(char, wsclient->data_len);
     } else if(wsclient->data_len < len+4) {
        wsclient->data_len = len + 4;
        wsclient->data = g_try_renew(char, wsclient->data, wsclient->data_len);
     }
     *(int *)wsclient->data = wsclient->id;
     g_memmove(wsclient->data+4, data, len);
     wsclient->client->send(wsclient->data, len+4, uWS::OpCode::BINARY);
 }

void wsclient_set_id(WsClientHandle handle,  int id)
{
    WsClient *wsclient = (WsClient *)handle;

    if(!handle) {
        g_print("%s(): Invalid WsClientHandle!!!\n",__func__);
        return;
    }
    wsclient->id = id;
    return;
}

int wsclient_get_id(WsClientHandle handle)
{
    WsClient *wsclient = (WsClient *)handle;

    if(!handle) {
        g_print("%s(): Invalid WsClientHandle!!!\n",__func__);
        return -1;
    }
    return wsclient->id;
}

/**
  * Pops data from the queue.
  * This function blocks until data become available.
  **/
MessageItem *wsclient_get_data(WsClientHandle handle)
{
    WsClient *wsclient = (WsClient *)handle;
    if(!handle) {
        g_print("%s(): Invalid WsClientHandle!!!\n",__func__);
        return NULL;
    }
    MessageItem *item =(MessageItem *)g_async_queue_pop(wsclient->message_queue);

    return item;
}

 /**
   * Pops data from the queue.
   * If no data is received before end_time, NULL is returned.
   **/
 MessageItem *wsclient_get_data_timed(WsClientHandle handle)
 {
     WsClient *wsclient = (WsClient *)handle;
     MessageItem *item = NULL;
     gint64 timeout_microsec = 400000; //400ms
     if(!handle) {
        g_print("%s(): Invalid WsClientHandle!!!\n",__func__);
        return NULL;
    }
     item = (MessageItem *)g_async_queue_timeout_pop(wsclient->message_queue, timeout_microsec);
     return item;
 }

void wsclient_free_item(MessageItem *item)
{
        item_free_func(item);
}
 void wsclient_destroy(WsClientHandle handle)
 {
     WsClient *wsclient = (WsClient *)handle;
 
     if(!handle)
         return;

     if(wsclient->client) {
        wsclient->client->close();
        g_usleep(1000);
     }

     if(wsclient->data)
         g_free(wsclient->data);

    if(wsclient->message_queue)
         g_async_queue_unref (wsclient->message_queue);
 
     g_free(handle);
 }
 
#ifdef __cplusplus
 };
#endif
