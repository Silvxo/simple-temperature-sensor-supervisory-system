#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdbool.h>

/**
 * @brief Inicializa o Wi-Fi no modo Station com as credenciais definidas em config.h.
 * Também inicia a sincronização de tempo via SNTP quando obtém IP.
 */
void wifi_init_sta(void);

/**
 * @brief Retorna se o Wi-Fi está conectado e com IP válido.
 */
bool wifi_is_connected(void);

#endif // WIFI_MANAGER_H
