#include <curl/curl.h> // Used for fetching data from URLs

#include <iomanip> // For formatting text outputs (e.g., setw() for setting field width)

#include <string>  // Defines the std::string class for handling strings

#include <iostream> // Provides facilities for input and output (e.g., std::cin, std::cout)

#include <vector> // Defines the std::vector class for dynamic arrays

#include <list> // Defines the std::list class for doubly-linked lists

#include <chrono> // Provides facilities for timekeeping (e.g., durations, clocks)

#include <map> // Defines the std::map class for sorted associative containers

#include <algorithm> // Provides algorithms for operations on containers (e.g., std::sort)

#include <stack> // Defines the std::stack class for stack data structure (LIFO)

#include <nlohmann/json.hpp> // Provides an easy way to work with JSON data (e.g., parsing, manipulating, serializing)

#include <fstream>


// RAPID is used for json as well 
#include "C:/Users/Kiirynn/source/repos/data.gov_filter/rapidjson/include/rapidjson/document.h"
#include "C:/Users/Kiirynn/source/repos/data.gov_filter/rapidjson/include/rapidjson/writer.h"
#include "C:/Users/Kiirynn/source/repos/data.gov_filter/rapidjson/include/rapidjson/stringbuffer.h"

using namespace std;
using json = nlohmann::json;

// Callback function to handle data (if you want to process response data)
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    // Here you can handle the data received from the server
    size_t totalSize = size * nmemb;
    string* jsonResponse = (string*)userp;
    jsonResponse->append((char*)contents, totalSize);
    return totalSize;
}


void printSalaries(const json& jsonData) {
    if (jsonData.is_object()) {
        // Print entire JSON structure for debugging
        cout << "JSON Object: " << jsonData.dump(4) << endl;


      
     // this here below to access specific data from the json
     // is basically the same thing in javascript ads example  "object.data[2].results.propteries"
    // accessing the values 

       // nlohmann::json :  the contains("key") & jsonData.is_array()/item.is_object() & get<string>/get<type>
        // RAPIDJSON     :  you would use document.HasMember("key") & document.IsArray()
        
        if (jsonData.contains("data") && jsonData["data"].is_array()) {
            for (const auto& item : jsonData["data"]) {
                if (item.is_object()) {
                   
                    string fieldName = item.contains("fieldName") ? item["fieldName"].get<string>() : "N/A";
                    string name = item.contains("name") ? item["name"].get<string>() : "N/A";

                
                    cout << "Field Name: " << fieldName << ", Name: " << name << endl;

                    // Extract and print the average base salary if available
                    if (fieldName == "average_of_base_salary") {
                        if (item.contains("value")) {
                            double salary = item["value"].get<double>();
                            cout << "Average Base Salary: $" << fixed << setprecision(2) << salary << endl;
                        }
                    }
                }
            }
        }
        else {
            cout << "Expected array not found in JSON data." << endl;
        }
    }
    else {
        cout << "JSON data is not an object." << endl;
    }
}




int main() {
    // Initialize CURL
    CURL* curl;
    CURLcode res;
    string readBuffer;

    // Start CURL session
    curl = curl_easy_init();
    if (curl == NULL) {
        cerr << "CURL initialization failed" << endl;
        return -1;
    }

   // set the URL to fetch from
    curl_easy_setopt(curl, CURLOPT_URL, "https://data.montgomerycountymd.gov/api/views/48wg-fkab");

    struct curl_slist* headers = NULL; // no headers set
    // set custom headers
    headers = curl_slist_append(headers, "X-Api-Key: cugzBQJI6qEESsirzOhxlRmvftXSIfliZkAGW5a3");
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // httpheader applies custom headers
    // write function makes it so you can call a function within curl 
    // writedata just applies the response data to the variable
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    // Perform the request
    res = curl_easy_perform(curl);

    // if the request is not ok do this
    if (res != CURLE_OK) {
        cerr << "Curl Error: " << curl_easy_strerror(res) << endl;
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
        return -1;
    }

    // Clean up
    curl_easy_cleanup(curl);
    curl_slist_free_all(headers);

    // Parse JSON response
    json jsonData;
    try {
        jsonData = json::parse(readBuffer);
    }
    catch (const json::parse_error& e) {
        cerr << "JSON parse error: " << e.what() << endl;
        return -1;
    }


    // Print the raw response to understand the structure
/*    cout << "Raw Response: " << jsonData.dump(4) << endl;*/  // Pretty print with indent of 4 spaces


     // Access and display specific parts of the JSON data with a seperate fucntion 
    printSalaries(jsonData);
    
    return 0;
}
