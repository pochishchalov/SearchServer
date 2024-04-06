﻿// Release version of the search system with TF-IDF ranking
// Code Review #4

#include "document.h"
#include "paginator.h"
#include "read_input_functions.h"
#include "request_queue.h"
#include "search_server.h"
#include "string_processing.h"

#include <iostream>

using namespace std;

int main() 
try{
    SearchServer search_server("and in at"s);
    RequestQueue request_queue(search_server);
    search_server.AddDocument(1, "curly cat curly tail"s, DocumentStatus::ACTUAL, { 7, 2, 7 });
    search_server.AddDocument(2, "curly dog and fancy collar"s, DocumentStatus::ACTUAL, { 1, 2, 3 });
    search_server.AddDocument(3, "big cat fancy collar "s, DocumentStatus::ACTUAL, { 1, 2, 8 });
    search_server.AddDocument(4, "big dog sparrow Eugene"s, DocumentStatus::ACTUAL, { 1, 3, 2 });
    search_server.AddDocument(5, "big dog sparrow Vasiliy"s, DocumentStatus::ACTUAL, { 1, 1, 1 });

    // 1439 queries with zero results
    for (int i = 0; i < 1439; ++i) {
        request_queue.AddFindRequest("empty request");
    }
    
    // still 1439 queries with zero result
    const auto search_results_1 = request_queue.AddFindRequest("curly dog"s);
    int page_size = 2;
    const auto pages_1 = Paginate(search_results_1, page_size);
    // Displaying found documents page by page
    for (auto page = pages_1.begin(); page != pages_1.end(); ++page) {
        std::cout << *page << std::endl;
        std::cout << "Page break"s << std::endl;
    }
    // new day, first request deleted, 1438 requests with zero results
    request_queue.AddFindRequest("big collar"s);
    // first query deleted, 1437 queries with zero result
    request_queue.AddFindRequest("sparrow"s);
    std::cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << std::endl;

    const auto search_results_2 = search_server.FindTopDocuments("curly dog"s);
    const auto pages_2 = Paginate(search_results_2, page_size);
    // Displaying found documents page by page
    for (auto page = pages_2.begin(); page != pages_2.end(); ++page) {
        std::cout << *page << std::endl;
        std::cout << "Page break"s << std::endl;
    }
    return 0;
}

catch (const invalid_argument& e) {
    cout << "Error: "s << e.what() << endl;
}

catch (const out_of_range& e) {
    cout << "Error: "s << e.what() << endl;
}

catch (...) {
    cout << "Unknown exception!"s <<  endl;
}