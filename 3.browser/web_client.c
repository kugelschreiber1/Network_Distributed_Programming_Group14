#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

// Callback function to write retrieved data into a buffer
size_t write_callback(void *ptr, size_t size, size_t nmemb, void *userdata) {
    // Print the retrieved data to stdout
    fwrite(ptr, size, nmemb, stdout);
    return size * nmemb;
}

int main() {
    CURL *curl;
    CURLcode res;

    // Initialize libcurl
    curl_global_init(CURL_GLOBAL_DEFAULT);

    // Create a curl handle
    curl = curl_easy_init();
    if (curl) {
        char url[256];

        // Get the URL from the user
        printf("Enter the URL: ");
        fgets(url, sizeof(url), stdin);

        // Remove the newline character from the URL
        size_t len = strlen(url);
        if (url[len - 1] == '\n') {
            url[len - 1] = '\0';
        }

        // Set the URL to retrieve
        curl_easy_setopt(curl, CURLOPT_URL, url);

        // Set the callback function to write the retrieved data
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

        // Perform the request
        res = curl_easy_perform(curl);

        // Check for errors
        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        // Cleanup
        curl_easy_cleanup(curl);
    }

    // Cleanup global libcurl resources
    curl_global_cleanup();

    return 0;
}
