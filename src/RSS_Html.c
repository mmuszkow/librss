#include "RSS.h"
#include "RSS_Html.h"
#include "RSS_Buffer.h"

/** Parses &#1234; */
RSS_char RSS_html_entity_parse_dec(const RSS_char* str, size_t* pos) {
    RSS_char    dec[4];
    int         valid;
    size_t      j, dec_len;
    
    valid = 1;
    dec_len = 0;
    
    while(str[dec_len] != 0 && str[dec_len] != RSS_text(';'))
        dec_len++;

    /* not a valid dec */
    if(dec_len > 6 || dec_len < 4) {
        (*pos)++;
        return RSS_text('&');
    }

    for(j=0; j<dec_len - 2; j++) {
        dec[j] = str[j+2];
        if(dec[j] < L'0' || dec[j] > L'9') {
            valid = 0;
            break;
        }
        dec[j] -= 0x30;
    }

    if(valid == 1) {
        (*pos) += dec_len + 1;
#if defined(_WIN32) && defined(RSS_USE_WSTRING)
        switch(dec_len)    {
            case 3: return dec[0];
            case 4: return dec[0]*10 + dec[1];
            case 5: return dec[0]*100 + dec[1]*10 + dec[2];
            default: return dec[0]*1000 + dec[1]*100 + dec[2]*10 + dec[3];
        }
#else
        /* TODO: not implemented */
            return RSS_text('?');
#endif
    } else {
        (*pos)++;
        return RSS_text('&');
    }
}

/** Parses &#x1234; */
RSS_char RSS_html_entity_parse_hex(const RSS_char* str, size_t* pos) {
    RSS_char    hex[4];
    int         valid;
    size_t      j, hex_len;
    
    valid = 1;
    hex_len = 0;
    
    while(str[hex_len] != 0 && str[hex_len] != RSS_text(';'))
        hex_len++;

    /* not a valid dec */
    if(hex_len > 7 || hex_len < 4) {
        (*pos)++;
        return RSS_text('&');
    }

    for(j=0; j<hex_len - 3; j++) {
        hex[j] = str[j+3];
        if(hex[j] >= L'0' && hex[j] <= L'9')
            hex[j] -= 0x30;
        else if(hex[j] >= L'A' && hex[j] <= L'F')
            hex[j] -= 55;
        else if(hex[j] >= L'a' && hex[j] <= L'f')
            hex[j] -= 87;
        else {
            valid = 0;
            break;
        }
    }

    if(valid == 1) {
        (*pos) += hex_len + 1;
#if defined(_WIN32) && defined(RSS_USE_WSTRING)
        switch(hex_len)    {
            case 4: return hex[0];
            case 5: return (hex[0]<<4) + hex[1];
            case 6: return (hex[0]<<8) + (hex[1]<<4) + hex[2];
            default: return (hex[0]<<12) + (hex[1]<<8) + (hex[2]<<4) + hex[3];
        }
#else
        /* TODO: not implemented */
        return RSS_text('?');
#endif
    } else {
        (*pos)++;
        return RSS_text('&');
    }
}

/** Html decode */
RSS_char* RSS_html_decode(const RSS_char* str) {
    size_t      pos;
    size_t      length;
    int         i;
    RSS_Buffer* temp;
    RSS_char*   res;

    length = RSS_strlen(str);

    if(length == 0)
        return NULL;

    temp = RSS_create_buffer();
    pos = 0;

    while(pos < length) {
        // Trim start
        if (temp->len == 0)
        {
            switch(str[pos]) {
                case RSS_text('\r'):
                case RSS_text('\n'):
                case RSS_text('\t'):
                case RSS_text(' '):
                    pos++;
                    continue;
            }
        }
        
        if(str[pos] == RSS_text('&')) {
            if(pos + 2 < length && str[pos+1] == RSS_text('#')) {
                /* &#x1A4F */
                if(str[pos+2] == RSS_text('x') || str[pos+2] == RSS_text('X'))
                    RSS_add_buffer(temp, RSS_html_entity_parse_hex(&str[pos], &pos));
                /* &#1234; */
                else
                    RSS_add_buffer(temp, RSS_html_entity_parse_dec(&str[pos], &pos));
            }
            /* &quot; etc */
            else {
                for(i=0; i<RSS_HTML_ENTITIES_TABLE_LEN; i++) {
                    if(pos + entities[i].len - 1 < length && 
                        RSS_strncasecmp(&str[pos], entities[i].encoded, entities[i].len) == 0) {
                        RSS_add_buffer(temp, entities[i].decoded);
                        pos += entities[i].len;
                        break;
                    }
                }
                if(i == RSS_HTML_ENTITIES_TABLE_LEN) {
                    RSS_add_buffer(temp, str[pos]);
                    pos++;
                }
            }
        } else {
            RSS_add_buffer(temp, str[pos]);
            pos++;
        }
    }

    // Only whitespace characters in input
    if (temp->len == 0)
    {
       RSS_free_buffer(temp);
       return NULL;
    }
    
    res = RSS_strdup(temp->str);
    RSS_free_buffer(temp);
    return res;
}

RSS_char* RSS_html_strip_tags(const RSS_char* str)
{
    RSS_Buffer* temp;
    size_t      pos;
    int         text;
    RSS_char*   plain;

    if(!str)
        return NULL;

    if(RSS_strstr(str, RSS_text("<")) == NULL)
        return RSS_strdup(str);

    temp = RSS_create_buffer();
    pos = 0;
    text = 1;
    while(str[pos] != 0) {
        if(str[pos] == RSS_text('<'))
            text = 0;
        else if(str[pos] == RSS_text('>'))
            text = 1;
        else if(text == 1)
            RSS_add_buffer(temp, str[pos]);

        pos++;
    }

    if(temp->len > 0)
        plain = RSS_strdup(temp->str);
    else
        plain = NULL;

    RSS_free_buffer(temp);

    return plain;
}

