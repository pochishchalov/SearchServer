#pragma once

#include "search_server.h"

#include <deque>
#include <vector>

class RequestQueue {
public:
    explicit RequestQueue(const SearchServer& search_server);
    
    template <typename DocumentPredicate>
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate);
    std::vector<Document> AddFindRequest(const std::string& raw_query, DocumentStatus status);
    std::vector<Document> AddFindRequest(const std::string& raw_query);

    int GetNoResultRequests() const;

private:
    struct QueryResult {
        std::vector<Document> find_documents_;
        bool is_empty_;
    };
    std::deque<QueryResult> requests_;
    const static int min_in_day_ = 1440;
    size_t minute_counter_ = 0;
    int empty_requests_ = 0;
    const SearchServer& search_server_;

    void AddRequest(const std::vector<Document>& request, const bool is_empty_request);
};

template <typename DocumentPredicate>
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentPredicate document_predicate) {
    const auto request = search_server_.FindTopDocuments(raw_query, document_predicate);
    AddRequest(request, request.empty());
    return request;
}