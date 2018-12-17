// Hass.io Pluggable Authentication Modules (PAM)
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// pam
#include <security/pam_modules.h>

// libcurl
#include <curl/curl.h>

/* expected hook */
PAM_EXTERN int pam_sm_setcred( pam_handle_t *pamh, int flags, int argc, const char **argv ) {
    return PAM_SUCCESS;
}

PAM_EXTERN int pam_sm_acct_mgmt(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    return PAM_SUCCESS;
}

static int writeFn(void* buf, size_t len, size_t size, void* userdata) {
	return len * size;
}

/* Function to handle stuff from HTTP response. */
static int callHassio(const char* pUsername, const char* pPassword) {
    CURL* pCurl = curl_easy_init();
    struct curl_slist *pHeader = NULL;
	
    char hassio_token[128];
    int res = -1;
    char *pEncUsername = curl_easy_escape(pCurl, pUsername, 0);
    char *pEncPassword = curl_easy_escape(pCurl, pPassword, 0);
    size_t fields_size = strlen(pEncUsername) + strlen(pEncPassword) + 21;
    char *pFields = malloc(fields_size);
    
    // Hass.io Token
    snprintf(hassio_token, 128, "X-Hassio-Key: %s", getenv("HASSIO_TOKEN"));
    pHeader = curl_slist_append(pHeader, hassio_token);

    // prepare
    snprintf(pFields, fields_size, "username=%s&password=%s", pEncUsername, pEncPassword);

    // Base setup
    curl_easy_setopt(pCurl, CURLOPT_URL, "http://hassio/auth");
    curl_easy_setopt(pCurl, CURLOPT_POST, 1L);
    curl_easy_setopt(pCurl, CURLOPT_HTTPHEADER, pHeader);

    // Data setup
    curl_easy_setopt(pCurl, CURLOPT_POSTFIELDS, pFields);

    // cURL setup
    curl_easy_setopt(pCurl, CURLOPT_WRITEFUNCTION, writeFn);
    curl_easy_setopt(pCurl, CURLOPT_NOPROGRESS, 1L);
    curl_easy_setopt(pCurl, CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(pCurl, CURLOPT_TIMEOUT, 5);

    // synchronous, but we don't really care
    res = curl_easy_perform(pCurl);

    // Cleanup
    curl_easy_cleanup(pCurl);
    curl_slist_free_all(pHeader);
    curl_free(pEncUsername);
    curl_free(pEncPassword);
    free(pFields);

    return res;
}

/* expected hook, this is where custom stuff happens */
PAM_EXTERN int pam_sm_authenticate(pam_handle_t* pamh, int flags, int argc, const char **argv) {
    int ret = 0;
    const char* pUsername = NULL;

    struct pam_message msg;
    struct pam_conv* pItem;
    struct pam_response* pResp;
    const struct pam_message* pMsg = &msg;

    msg.msg_style = PAM_PROMPT_ECHO_OFF;
    msg.msg = "Hass.io Auth: ";

    if (pam_get_user(pamh, &pUsername, NULL) != PAM_SUCCESS) {
        return PAM_AUTH_ERR;
    }

    if (pam_get_item(pamh, PAM_CONV, (const void**)&pItem) != PAM_SUCCESS || !pItem) {
        fprintf(stderr, "Couldn't get pam_conv\n");
        return PAM_AUTH_ERR;
    }

    pItem->conv(1, &pMsg, &pResp, pItem->appdata_ptr);

    ret = PAM_SUCCESS;
    if (callHassio(pUsername, pResp[0].resp) != 0) {
        ret = PAM_AUTH_ERR;
    }

    memset(pResp[0].resp, 0, strlen(pResp[0].resp));
    free(pResp);

    return ret;
}
