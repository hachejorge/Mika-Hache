#ifndef SRV_LOGS_H
#define SRV_LOGS_H

// Niveles de depuraci�n
#define LOG_LEVEL_NONE   0  // No se genera ning�n mensaje
#define LOG_LEVEL_ERROR  1  // Solo errores
#define LOG_LEVEL_INFO   2  // Errores e informaci�n
#define LOG_LEVEL_DEBUG  3  // Errores, informaci�n y depuraci�n detallada

// Configuraci�n de nivel actual
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

// Macros para manejo de niveles de log
#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOG_ERROR(format, ...) printf("[ERROR] " format "\n", ##__VA_ARGS__)
#else
#define LOG_ERROR(format, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOG_INFO(format, ...) printf("[INFO] " format "\n", ##__VA_ARGS__)
#else
#define LOG_INFO(format, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOG_DEBUG(format, ...) printf("[DEBUG] " format "\n", ##__VA_ARGS__)
#else
#define LOG_DEBUG(format, ...)
#endif

#endif // SRV_LOGS_H
