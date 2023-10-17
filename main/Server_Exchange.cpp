/*
 * Server_Exchange.cpp
 *
 *  Created on: 7 сент. 2023 г.
 *      Author: boyarkin.k
 */
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_netif.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>
#include "esp_log.h"
#include "protocol.h"


extern float Temperature;
extern float Humidity;
extern uint8_t   addr;

#define HOST_IP_ADDR "109.194.141.27"
#define PORT		 34004

static const char *TAG = "Server_Exchange";

void Socket_Recv(void *pvParameter)
{
		char rx_buffer[128];
	    char addr_str[128];
	    int ip_protocol = 0;
	    struct sockaddr_in6 dest_addr;

        struct sockaddr_in *dest_addr_ip4 = (struct sockaddr_in *)&dest_addr;
        dest_addr_ip4->sin_addr.s_addr = htonl(INADDR_ANY);
        dest_addr_ip4->sin_family = AF_INET;
        dest_addr_ip4->sin_port = htons(PORT);
        ip_protocol = IPPROTO_IP;

	    int sock = socket( AF_INET, SOCK_DGRAM, ip_protocol);

	    if (sock < 0) {
	        ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
	        return;
	      }

	    ESP_LOGI(TAG, "Socket created");

       // Set timeout
       struct timeval timeout;
       timeout.tv_sec = 10;
       timeout.tv_usec = 0;
       setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof timeout);

       int err = bind(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
       if (err < 0) {
           ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
       }
       ESP_LOGI(TAG, "Socket bound, port %d", PORT);

       struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
       socklen_t socklen = sizeof(source_addr);

	        while (1) {

	            int len = recvfrom(sock, rx_buffer, sizeof(rx_buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);

	            // Error occurred during receiving
	            if (len < 0) {
	                ESP_LOGE(TAG, "recvfrom failed: errno %d", errno);
	            }
	            // Data received
	            else {
	                // Get the sender's ip address as string
	                if (source_addr.ss_family == PF_INET) {
	                    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, addr_str, sizeof(addr_str) - 1);
	                }

	                if (((SSensorData*)rx_buffer)->type==TYPE_GET_DATA)
	                {

	                	SSensorData data;
	                	data.type=TYPE_DATA;
	                	data.sensor_addr=addr;
	                	data.temperature=Temperature;
	                	data.humidity=Humidity;

	                	int err = sendto(sock, &data, sizeof(data), 0, (struct sockaddr *)&source_addr, sizeof(source_addr));
		                if (err < 0) {
		                    ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
		                    break;

		                			}

	                }

	            }
	        }

	        if (sock != -1) {
	            ESP_LOGE(TAG, "Shutting down socket and restarting...");
	            shutdown(sock, 0);
	            close(sock);
	        }

	    vTaskDelete(NULL);



}

