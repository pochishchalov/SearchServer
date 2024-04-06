#include "request_queue.h"

RequestQueue::RequestQueue(const SearchServer& search_server)
    :search_server_(search_server)
{
}

int RequestQueue::GetNoResultRequests() const {
    return empty_requests_;
}

void RequestQueue::AddRequest(const std::vector<Document>& request, const bool is_empty_request) {
    ++minute_counter_;
    if (minute_counter_ > min_in_day_) {
        if (requests_.front().is_empty_) {
            --empty_requests_;
        }
        requests_.pop_front();
    }

    if (is_empty_request) {
        ++empty_requests_;
    }
    requests_.push_back({ request, is_empty_request });
}

std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query, DocumentStatus status) {
    const auto request = search_server_.FindTopDocuments(raw_query, status);
    AddRequest(request, request.empty());
    return request;
}
std::vector<Document> RequestQueue::AddFindRequest(const std::string& raw_query) {
    const auto request = search_server_.FindTopDocuments(raw_query);
    AddRequest(request, request.empty());
    return request;
}