#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "document.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"

using namespace rapidxml;
using namespace std::literals;

struct ReaderDocument {
    int id = 0;
    DocumentStatus status = DocumentStatus::ACTUAL;
    std::string text;
    std::vector<int> ratings;
};

enum class RequestType {
    FIND,
    TOP,
    NO_RESULT,
    MATCH
};

struct Request {
    std::string name;
    int id = 0;
    RequestType type;
};

class Reader {
public:
    void LoadXML(const std::string& file_name);
    std::vector<ReaderDocument>& GetDocuments() { return documents_; };
    std::vector<Request>& GetRequests() { return requests_; };
    std::string& GetStopWords() { return stop_words_; };
    int GetPageSize() const { return page_size_; };
private:
    std::vector<ReaderDocument> documents_;
    std::vector<Request> requests_;
    std::string stop_words_;
    int page_size_ = 1;
};

static ReaderDocument AddDocument(node_iterator<char> iter) {
    ReaderDocument result;
    for (xml_attribute<>* attr = iter->first_attribute();
        attr; attr = attr->next_attribute())
    {
        if (attr->name() == "id"s) {
            result.id = std::stoi(attr->value());
            if (result.id < 0) {
                throw std::invalid_argument("negative id"s);
            }
        }
        else if (attr->name() == "text"s) {
            result.text = std::move(attr->value());
        }
        else if (attr->name() == "status"s) {
            if (attr->value() == "irrelevant"s) {
                result.status = DocumentStatus::IRRELEVANT;
            }
            else if (attr->value() == "banned"s) {
                result.status = DocumentStatus::BANNED;
            }
            else if (attr->value() == "removed"s) {
                result.status = DocumentStatus::REMOVED;
            }
            else {
                result.status = DocumentStatus::ACTUAL;
            }
        }
        if (attr->name() == "ratings"s) {
            std::string ratings = std::move(attr->value());
            auto first = ratings.begin(), last = ratings.begin();
            for (; last != ratings.end(); ++last)
            {
                if (*last == ',') {
                    result.ratings.emplace_back(std::stoi(std::string(first, last)));
                    ++last;
                    first = last;
                }
            }
            result.ratings.emplace_back(std::stoi(std::string(first, last)));
        }
    }
    return result;
}

void Reader::LoadXML(const std::string& file_name) {
    std::string text;
    std::string buffer;;
    std::ifstream in(file_name);
    if (!in) {
        throw std::invalid_argument("can't open "s + file_name);
    }
    while (std::getline(in, buffer)) {
        text += buffer;
    }
    xml_document<> doc;    // character type defaults to char
    doc.parse<0>(text.data());    // 0 means default parse flags
    xml_node<>* node = doc.first_node();
    if (node->name() != "requests"s) {
        throw std::invalid_argument("xml file incorrect"s);
    }
    try {
        for (node_iterator iter = node; iter; ++iter) {
            if (iter->name() == "document"s) {
                documents_.emplace_back(AddDocument(iter));
            }
            else if (iter->name() == "stop_words"s) {
                stop_words_ = iter->value();
            }
            else if (iter->name() == "page_size"s) {
                page_size_ = std::stoi(iter->value());
                if (page_size_ < 1) {
                    throw std::invalid_argument("page size incorrect"s);
                }
            }
            else if (iter->name() == "find_request"s) {
                requests_.emplace_back(Request{ iter->value(), 0, RequestType::FIND });
            }
            else if (iter->name() == "top_documents"s) {
                requests_.emplace_back(Request{ iter->value(), 0, RequestType::TOP });
            }
            else if (iter->name() == "match"s) {
                Request request;
                request.type = RequestType::MATCH;
                xml_attribute<>* attr = iter->first_attribute();
                if (attr->name() == "id"s) {
                    request.id = std::stoi(attr->value());
                    request.name = iter->last_attribute()->value();
                }
                else {
                    request.id = std::stoi(iter->last_attribute()->value());
                    request.name = attr->value();
                }
                requests_.push_back(std::move(request));
            }
            else if (iter->name() == "no_result"s) {
                requests_.emplace_back(Request{ ""s, 0, RequestType::NO_RESULT });
            }
        }
    }
    catch (...) {
        throw std::invalid_argument("xml file incorrect"s);
    }
}