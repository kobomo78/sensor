idf_component_register(	SRCS "station_example_main.cpp"
						SRCS "wifi.c"
						SRCS "blynk_management.c"
						SRCS "ota.cpp"
						SRCS "Server_Exchange.cpp"
                    	INCLUDE_DIRS "."
                    	INCLUDE_DIRS "../include"
                    	INCLUDE_DIRS "../../switch/include"
                    	REQUIRES DHT22 blynk esp_wifi esp_https_ota nvs_flash app_update
                    	EMBED_FILES ${PROJECT_DIR}/cert/ca_cert.pem)

