
#include <curl/curl.h> // used for fetching

#include <iomanip> // Provides facilities for formatting text outputs (e.g.,item.value, setw() for setting field width, left for left alignment)

#include <string>  // Defines the std::string class for handling strings

#include <iostream> // Provides facilities for input and output (e.g., std::cin, std::cout)

#include <vector> // Defines the std::vector class for dynamic arrays

#include <list> // Defines the std::list class for doubly-linked lists

#include <chrono> // Provides facilities for timekeeping (e.g., durations, clocks)

#include <map> // Defines the std::map class for sorted associative containers

#include <algorithm> // Provides algorithms for operations on containers (e.g., std::sort)

#include <stack> // Defines the std::stack class for stack data structure (LIFO)

#include <nlohmann/json.hpp> // Provides an easy way to work with JSON data (e.g., parsing, manipulating, serializing)

using namespace std;
using json = nlohmann::json;



//  Callback function to handle data (if you want to process response data)

size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp) {
    
    
    // Here you can handle the data received from the server
    // stores the data in a string
    ((string*)userp)->append((char*)contents, size * nmemb);

    return size * nmemb; // returns the total size of the chunk
}

int main()
{
   // ------> curl is the request object that gets the callback function set as the parameter,
   // then the curl_easy_perform is where its executed and makes the call

    // CURL means client url
    // Stores the CURL handle used to manage the request
    CURL* curl;

    // stores return value in a var
    CURLcode result;

    // initialize a buffer string that will later store 
    string readBuffer;

    // Starts the session, return the curl handle we'll use to setup the request
    curl = curl_easy_init();

    // if the curl == null its has failed to request 
    if (curl == NULL) {
        fprintf(stderr, "HTTP request failed\n");
        return -1;

    };

 
        // curl_easy_setopt() is used to set the options for the request, 
        // get the corrosponding indetifier for that database 
        curl_easy_setopt(curl, CURLOPT_URL, "https://data.montgomerycountymd.gov/api/views/48wg-fkab");


            // This declares a pointer named headers of type struct curl_slist* .
                 // curl_slist is a data structure provided by libcurl to handle lists of strings, which in this case are HTTP headers.

                    // Initializes the headers pointer to NULL.
                         // Setting it to NULL is important because it indicates that the list is currently empty.

        struct curl_slist* headers = NULL;
        headers = curl_slist_append(headers, "X-Api-Key: cugzBQJI6qEESsirzOhxlRmvftXSIfliZkAGW5a3");
        headers = curl_slist_append(headers, "Content-Type: application/json");


        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // CURLOPT_WRITEFUNCTION must be used to have a function called
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);

        // tells libcurl put the data from the request into the readBuffer string.
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer); 

        // makes the actual fetch call 
        result = curl_easy_perform(curl);

        if (result != CURLE_OK) {
            fprintf(stderr, "Error: %s\n", curl_easy_strerror(result));

            return -1;
        };

        // if everything goes as planned this pretty much cleans it in memory after the request is made
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);


        // Parse JSON
        try {
            json jsonData = json::parse(readBuffer);

            // Display data in a table format
            cout << left << setw(30) << "Job Title" << " | " << setw(15) << "Class Code" << " | " << setw(10) << "Grade" << " | " << "Average Salary" << endl;
            cout << "-------------------------------------------------------------------------------" << endl;

            // Example JSON data: [{"name": "John", "age": 30}, {"name": "Jane", "age": 25}]

            for (const auto& item : jsonData) {
                string positionTitle = item.value("position_title", "N/A");
                int classCode = item.value("position_class_code", 0);
                string grade = item.value("grade", "N/A");
                string avgSalary = item.value("average_of_base_salary", "N/A");

                cout << left << setw(30) << positionTitle << " | " << setw(15) << classCode << " | " << setw(10) << grade << " | " << avgSalary << endl;
            }
        }
        catch (json::parse_error& e) {
            cerr << "JSON parsing error: " << e.what() << endl;
            return -1;
        }

    
    return 0;
}
