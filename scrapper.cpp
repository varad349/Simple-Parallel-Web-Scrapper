#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <omp.h>
#include <curl/curl.h>

size_t write_callback(char* ptr, size_t size, size_t nmemb, std::ofstream* file) {
 if (file) {
 file->write(ptr, size * nmemb);
 return size * nmemb;
 }
 return 0;
}
void scrape_websites(const std::vector<std::string>& urls) {
 #pragma omp parallel for
 for (int i = 0; i < urls.size(); ++i) {
 CURL* curl = curl_easy_init();
 if (!curl) {
 std::cerr << "Failed to initialize libcurl for thread" << std::endl;
 continue;
 }

 int thread_num = omp_get_thread_num();
 std::cout << "Thread " << thread_num << " is fetching data from " <<urls[i] << std::endl;
std::string scraped_data;

 curl_easy_setopt(curl, CURLOPT_URL, urls[i].c_str());
 std::string filename = "scraped_data_" + std::to_string(i) + ".txt";
 std::ofstream outfile(filename);
 curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
 curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outfile);

 CURLcode res = curl_easy_perform(curl);
 if (res != CURLE_OK) {
 std::cerr << "Failed to fetch data from " << urls[i] << ": " <<
curl_easy_strerror(res) << std::endl;
 }

 curl_easy_cleanup(curl);

 outfile.close();
 }
}
int main() {
 curl_global_init(CURL_GLOBAL_ALL);

 std::vector<std::string> urls = {
 "https://bloclibrary.dev/",
 "https://ctftime.org/event/2229",
 "https://www.kali.org/docs/"
 };

 scrape_websites(urls);

 curl_global_cleanup();

 return 0;
}