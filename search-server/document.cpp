#include "document.h"

using namespace std;

Document::Document(int id, double relevance, int rating)
    : id(id)
    , relevance(relevance)
    , rating(rating) {
}

ostream& operator<<(ostream& os, const Document& document) {
    os << "{ "s
        << "document_id = "s << document.id << ", "s
        << "relevance = "s << document.relevance << ", "s
        << "rating = "s << document.rating << " }"s;
    return os;
}

ostream& operator<<(ostream& os, const DocumentStatus status) {
    switch (status)
    {
    case DocumentStatus::ACTUAL: os << "actual"s;
        break;
    case DocumentStatus::BANNED: os << "banned"s;
        break;
    case DocumentStatus::IRRELEVANT: os << "irrelevant"s;
        break;
    case DocumentStatus::REMOVED: os << "removed"s;
        break;
    }
    return os;
}