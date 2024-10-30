#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

int32_t is_ascii(const char *str) {
    while (*str) {
        if ((unsigned char)*str > 127) {
            return 0;
        }
        str++;
    }
    return 1;
}

int32_t capitalize_ascii(char *str) {
    int32_t count = 0;
    while (*str) {
        if (islower((unsigned char)*str)) {
            *str = toupper((unsigned char)*str);
            count++;
        }
        str++;
    }
    return count;
}

int32_t width_from_start_byte(char start_byte) {
    if ((start_byte & 0x80) == 0) {
        return 1;
    } else if ((start_byte & 0xE0) == 0xC0) {
        return 2;
    } else if ((start_byte & 0xF0) == 0xE0) {
        return 3;
    } else if ((start_byte & 0xF8) == 0xF0) {
        return 4;
    }
    return -1;
}

int32_t utf8_strlen(const char *str) {
    int32_t len = 0;
    while (*str) {
        int width = width_from_start_byte(*str);
        if (width == -1) return -1;
        str += width;
        len++;
    }
    return len;
}

int32_t codepoint_index_to_byte_index(const char *str, int32_t cpi) {
    int32_t byte_idx = 0;
    while (*str && cpi > 0) {
        int width = width_from_start_byte(*str);
        if (width == -1) return -1;
        str += width;
        byte_idx += width;
        cpi--;
    }
    return byte_idx;
}

void utf8_substring(const char *str, int32_t cpi_start, int32_t cpi_end, char *result) {
    int32_t byte_idx = codepoint_index_to_byte_index(str, cpi_start);
    str += byte_idx;
    while (cpi_start < cpi_end && *str) {
        int width = width_from_start_byte(*str);
        if (width == -1) break;
        memcpy(result, str, width);
        str += width;
        result += width;
        cpi_start++;
    }
    *result = '\0';
}

int32_t codepoint_at(const char *str, int32_t cpi) {
    int32_t byte_idx = codepoint_index_to_byte_index(str, cpi);
    str += byte_idx;
    int width = width_from_start_byte(*str);
    int32_t codepoint = 0;
    if (width == 1) {
        codepoint = (unsigned char)*str;
    } else if (width == 2) {
        codepoint = ((str[0] & 0x1F) << 6) | (str[1] & 0x3F);
    } else if (width == 3) {
        codepoint = ((str[0] & 0x0F) << 12) | ((str[1] & 0x3F) << 6) | (str[2] & 0x3F);
    } else if (width == 4) {
        codepoint = ((str[0] & 0x07) << 18) | ((str[1] & 0x3F) << 12) | ((str[2] & 0x3F) << 6) | (str[3] & 0x3F);
    }
    return codepoint;
}

void next_utf8_char(const char *str, int32_t cpi, char *result) {
    int32_t current_codepoint = codepoint_at(str, cpi);
    current_codepoint++;

    if (current_codepoint <= 0x7F) {
        result[0] = current_codepoint;
        result[1] = '\0';
    } else if (current_codepoint <= 0x7FF) {
        result[0] = 0xC0 | ((current_codepoint >> 6) & 0x1F);
        result[1] = 0x80 | (current_codepoint & 0x3F);
        result[2] = '\0';
    } else if (current_codepoint <= 0xFFFF) {
        result[0] = 0xE0 | ((current_codepoint >> 12) & 0x0F);
        result[1] = 0x80 | ((current_codepoint >> 6) & 0x3F);
        result[2] = 0x80 | (current_codepoint & 0x3F);
        result[3] = '\0';
    } else if (current_codepoint <= 0x10FFFF) {
        result[0] = 0xF0 | ((current_codepoint >> 18) & 0x07);
        result[1] = 0x80 | ((current_codepoint >> 12) & 0x3F);
        result[2] = 0x80 | ((current_codepoint >> 6) & 0x3F);
        result[3] = 0x80 | (current_codepoint & 0x3F);
        result[4] = '\0';
    }
}

int is_animal_emoji(int32_t codepoint) {
    if (codepoint >= 0x1F400 && codepoint <= 0x1F43F) {
        return 1;
    }
    if (codepoint >= 0x1F980 && codepoint <= 0x1F9AE) {
        return 1;
    }
    return 0;
}

int main() {
    char str[100];
    printf("Enter a UTF-8 encoded string: ");
    fgets(str, sizeof(str), stdin);
    str[strcspn(str, "\n")] = '\0';
    printf("Valid ASCII: %s\n", is_ascii(str) ? "true" : "false");
    char temp_str[100];
    strcpy(temp_str, str);
    int updated = capitalize_ascii(temp_str);
    printf("Uppercased ASCII: \"%s\"\n", temp_str);
    printf("Length in bytes: %zu\n", strlen(str));
    int codepoint_len = utf8_strlen(str);
    printf("Number of code points: %d\n", codepoint_len);
    printf("Bytes per code point: ");
    for (int i = 0; i < codepoint_len; i++) {
        printf("%d ", width_from_start_byte(str[codepoint_index_to_byte_index(str, i)]));
    }
    printf("\n");
    printf("Substring of the first 6 code points: ");
    char result[100];
    utf8_substring(str, 0, codepoint_len < 6 ? codepoint_len : 6, result);
    printf("\"%s\"\n", result);
    printf("Code points as decimal numbers: ");
    for (int i = 0; i < codepoint_len; i++) {
        printf("%d ", codepoint_at(str, i));
    }
    printf("\n");
    printf("Animal emojis: ");
    for (int i = 0; i < codepoint_len; i++) {
        int32_t codepoint = codepoint_at(str, i);
        if (is_animal_emoji(codepoint)) {
            printf("%.*s", width_from_start_byte(str[codepoint_index_to_byte_index(str, i)]), str + codepoint_index_to_byte_index(str, i));
        }
    }
    printf("\n");
    int32_t idx = 3;
    next_utf8_char(str, idx, result);
    printf("Next Character of Codepoint at Index %d: %s\n", idx, result);
    return 0;
}
