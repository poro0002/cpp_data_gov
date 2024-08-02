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

// OPTIONS


// Callback function to handle data (if you want to process response data)
size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {

    // Here you can handle the data received from the server
    size_t totalSize = size * nmemb;

    // string* jsonResponse = (string*)userp; does not create a new variable; it simply casts the void* pointer (userp) to a string*
    string* jsonResponse = (string*)userp;

    // Append the received data to the string object pointed to by jsonResponse
    jsonResponse->append((char*)contents, totalSize); 

    //---<explanation>----
    // so jsonResponse is a string pointer that is userp and then gets the direction to memory for the string that holds the jsondata 
    
    return totalSize;
}



void logJobTitles(const json& jsonData) {


    if (jsonData.contains("data")) {
        const auto& data = jsonData["data"];

        if (data.is_array()) {
         /*   cout << "'data' is an array." << endl; */
            vector<string> vec;
            
            cout << "\n" << endl;
            for (const auto& item : data) {
                if (item.is_array() && item.size() > 8) {
                    string title = item[8].get<string>();  // for each peice of data it puts the "job title" into a title string variable

                  
                    vec.push_back(title);
                 

                }
            }

            sort(vec.begin(), vec.end());

                for (const auto& title : vec) {
                    cout << title << endl;
                };

         
        }
        else {
            cout << "'data' is not an array" << endl;
        }
    }
    else {
        cout << "Does not contain 'data'" << endl;
    }
};



void searchJobByTitle(const json& jsonData, const string& userInput) {

    /*cout << "json object: " << jsonData.dump(4) << endl;*/  // this is to log raw data





    if (jsonData.contains("data")) {
        const auto& data = jsonData["data"];

        if (data.is_array()) {
           /* cout << "'data' is an array." << endl;*/
            for (const auto& item : data) {
                if (item.is_array() && item.size() > 8) {
                    string title = item[8].get<string>();  // for each peice of data it puts the "job title" into a title string variable

                    // so this makes it so it will look to match the user string with the corrosponding match in the data
                    if (title.find(userInput) != string::npos) {
                        // Print the details of the job if it matches
                        cout << "Job Title: " << title << endl;
                        cout << "Code: " << item[1].get<string>() << endl;
                        cout << "Salary: " << item[11].get<string>() << endl;
                        cout << "Grade: " << item[10].get<string>() << endl;
                        cout << "Number: " << item[12].get<string>() << endl;
                        cout << endl;
                    }
                }
            }
         
        }
        else {
            cout << "'data' is not an array" << endl;
        }
    }
    else {
        cout << "Does not contain 'data'" << endl;
    }
};





int main() {



    // Initialize CURL and these curl keywords point to these variables that will hold coro data 
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
  // Set the URL to fetch JSON data
    curl_easy_setopt(curl, CURLOPT_URL, "https://data.montgomerycountymd.gov/api/views/48wg-fkab/rows.json?accessType=DOWNLOAD");

    struct curl_slist* headers = NULL; // no headers set
    // set custom headers
    headers = curl_slist_append(headers, "X-Api-Key: cugzBQJI6qEESsirzOhxlRmvftXSIfliZkAGW5a3");
    headers = curl_slist_append(headers, "Content-Type: application/json");

    // httpheader applies custom headers
    // write function makes it so you can call a function within curl 
    // then, writedata just applies the "jsonResponse" data in the callback function to the variable readBuffer 
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
        /* cout << "Fetched data: " <<  jsonData.dump(4) << endl;*/
    }
    catch (const json::parse_error& e) {
        cerr << "JSON parse error: " << e.what() << endl;
        return -1;
    }

 

    // Search for a specific job title
    string userInput;


    bool running = true;

    while (running) {
        cout << "\n" << endl;
        cout << "Use The Command /list to see the job options or exit with /exit" << endl;
        cout << "Enter the job title to search:";
        getline(cin, userInput); // this will only run if you click enter. once that finishes it will run the nest line in your code 

        if (userInput == "/list") {
            cout << "\n" << endl;
            logJobTitles(jsonData);
        }
        else if (userInput == "/exit") {
            running = false; // the while loop will quit running the job titles function and c-input 
        }
        else {
            cout << "\n" << endl;
            searchJobByTitle(jsonData, userInput); // you will the the user input value to use with the search function, obvi you will need the json aswell

        }
    };
   


    return 0;
};
