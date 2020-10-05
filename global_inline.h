//
// Created by cacoethes on 8/10/20.
//

#ifndef VULKAN_SPECS_GLOBAL_INLINE_H
#define VULKAN_SPECS_GLOBAL_INLINE_H

#include <iostream>
#include <vulkan/vulkan.h>
#include <cstring>

inline bool check_vk_result(int Result, const char* Error_string) {
    return (Result == 0) || std::cerr << Error_string << " With error code : " << Result << std::endl;
}
inline bool check_vk_count(uint32_t Count, const char* Notify_zero_string){
    return Count > 0 || std::cerr << "Number of " << Notify_zero_string << " is zero" << std::endl;
}
template<typename S, typename F>
inline bool check_vk_available(uint32_t* Count, VkResult (*fp)(uint32_t *,S *),
                               const char* Available_type_string, F S::*field, F *desired,
                               uint32_t num_desired, bool string_comp, uint8_t str_size){
    *Count = 0;
    char ret_num_report[sizeof("Retrieving ")/sizeof(char) + str_size + sizeof(" Count Failed")/sizeof(char) + 1];
    strcpy(ret_num_report,"Retrieving "); strcat(ret_num_report,Available_type_string); strcat(ret_num_report, " Count Failed");
    char num_report[str_size + 1];
    strcpy(num_report,Available_type_string); num_report[str_size] = 's';
    char ret_error_report[sizeof("Error in Available ")/sizeof(char)+ str_size + sizeof("s retrieval") + 1];
    strcpy(ret_error_report,"Error in Available "); strcat(ret_error_report,Available_type_string); strcat(ret_error_report, "s retrieval");
    check_vk_result((*fp)(Count,NULL),
                    ret_num_report);
    check_vk_count(*Count,
                   num_report);
    S available_type[*Count];
    check_vk_result((*fp)(Count,available_type),
                    ret_error_report);
    if (!string_comp){
        for (int j = 0; j < num_desired; ++j) {
            bool found = false;
            for (uint32_t i = 0; i < *Count; ++i) {
                if (available_type[i].*field == desired[j]) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    }
    else {
        for (int j = 0; j < num_desired; ++j) {
            bool found = false;
            for (uint32_t i = 0; i < *Count; ++i) {
                if (!strcmp(available_type[i].*field,desired[j])) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    }
}
template<typename T, typename S, typename F>
inline bool check_vk_available(uint32_t* Count, VkResult (*fp)(T,uint32_t *,S *),
                               const char* Available_type_string, T t, F S::*field, F *desired,
                               uint32_t num_desired, bool string_comp, uint8_t str_size){
    *Count = 0;
    char ret_num_report[sizeof("Retrieving ")/sizeof(char) + str_size + sizeof(" Count Failed")/sizeof(char) + 1];
    strcpy(ret_num_report,"Retrieving "); strcat(ret_num_report,Available_type_string); strcat(ret_num_report, " Count Failed");
    char num_report[str_size + 1];
    strcpy(num_report,Available_type_string); num_report[str_size] = 's';
    char ret_error_report[sizeof("Error in Available ")/sizeof(char)+ str_size + sizeof("s retrieval") + 1];
    strcpy(ret_error_report,"Error in Available "); strcat(ret_error_report,Available_type_string); strcat(ret_error_report, "s retrieval");
    check_vk_result((*fp)(t,Count,NULL),
                    ret_num_report);
    check_vk_count(*Count,
                   num_report);
    S available_type[*Count];
    check_vk_result((*fp)(t,Count,available_type),
                    ret_error_report);
    if (!string_comp){
        for (int j = 0; j < num_desired; ++j) {
            bool found = false;
            for (uint32_t i = 0; i < *Count; ++i) {
                if (available_type[i].*field == desired[j]) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    }
    else {
        for (int j = 0; j < num_desired; ++j) {
            bool found = false;
            for (uint32_t i = 0; i < *Count; ++i) {
                if (!strcmp(available_type[i].*field,desired[j])) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    }
}

template<typename T, typename U, typename S, typename F>
inline bool check_vk_available(uint32_t* Count, VkResult (*fp)(T,U,uint32_t *,S *),
                               const char* Available_type_string, T t, U u, F S::*field, F desired[],
                               uint32_t num_desired, bool string_comp, uint8_t str_size){
    *Count = 0;
    char ret_num_report[sizeof("Retrieving ")/sizeof(char) + str_size + sizeof(" Count Failed")/sizeof(char) + 1];
    strcpy(ret_num_report,"Retrieving "); strcat(ret_num_report,Available_type_string); strcat(ret_num_report, " Count Failed");
    char num_report[str_size + 1];
    strcpy(num_report,Available_type_string); num_report[str_size] = 's';
    char ret_error_report[sizeof("Error in Available ")/sizeof(char)+ str_size + sizeof(" retrieval") + 1];
    strcpy(ret_error_report,"Error in Available "); strcat(ret_error_report,Available_type_string); strcat(ret_error_report, " retrieval");
    check_vk_result((*fp)(t,u,Count,NULL),
                    ret_num_report);
    check_vk_count(*Count,
                   num_report);
    S available_type[*Count];
    check_vk_result((*fp)(t,u,Count,available_type),
                    ret_error_report);
    if (!string_comp){
        for (int j = 0; j < num_desired; ++j) {
            bool found = false;
            for (uint32_t i = 0; i < *Count; ++i) {
                if (available_type[i].*field == desired[j]) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    }
    else {
        for (int j = 0; j < num_desired; ++j) {
            bool found = false;
            for (uint32_t i = 0; i < *Count; ++i) {
                if (!strcmp(available_type[i].*field,desired[j])) {
                    found = true;
                    break;
                }
            }
            if (!found) {
                return false;
            }
        }
        return true;
    }
}

inline bool ifree(void* ptr){
    if (ptr) {
        free(ptr);
        return true;
    } else {
        return false;
    }
}

inline const char *DeviceTypeStr(VkPhysicalDeviceType deviceType) {
    switch (deviceType) {
        case VK_PHYSICAL_DEVICE_TYPE_CPU :
            return "CPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU :
            return "Discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU :
            return "Integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU :
            return "Virtual GPU";
        default:
            return "unexpected";
    }
}

inline uint32_t Min(uint32_t a, uint32_t b){
//    if(!a){
//        return b;
//    }
    return (a < b) ? a : b;
}

inline uint32_t Max(uint32_t a, uint32_t b){
    return (a > b) ? a : b;
}
#endif //VULKAN_SPECS_GLOBAL_INLINE_H
