#include "document.h"
#include "paginator.h"
#include "reader.h"
#include "read_input_functions.h"
#include "request_queue.h"
#include "search_server.h"
#include "string_processing.h"

#include <iostream>
#include <string>
#include <conio.h>
#include <filesystem>

using namespace std;

void AddDocuments(Reader& reader, SearchServer& search_server) {
    for (auto& doc : reader.GetDocuments()) {
        search_server.AddDocument(doc.id, move(doc.text), doc.status, move(doc.ratings));
    }
}

void PrintPages(const vector<Document>& documents, int page_size) {
    for (const auto page : Paginate(documents, page_size)) {

        cout << page << endl;
        cout << "Page break"s << endl;
    }
}

void PrintMatchDocumentResult(int document_id, const vector<string>& words, DocumentStatus status) {
    cout << "{ "s
        << "document_id = "s << document_id << ", "s
        << "status = "s << status << ", "s
        << "words ="s;
    for (const string& word : words) {
        cout << ' ' << word;
    }
    cout << " }"s << endl;
    cout << "Page break"s << endl;
}

void PrintResults(Reader& reader, SearchServer& search_server, RequestQueue& request_queue) {
    for (auto request : reader.GetRequests()) {
        switch (request.type)
        {
        case RequestType::FIND:
        {
            cout << "Find request: \'" << request.name << '\'' << endl;
            PrintPages(request_queue.AddFindRequest(move(request.name)), reader.GetPageSize());
            break;
        }
        case RequestType::TOP:
        {
            cout << "Top request: \'" << request.name << '\'' << endl;
            PrintPages(search_server.FindTopDocuments(move(request.name)), reader.GetPageSize());
            break;
        }
        case RequestType::MATCH:
        {
            cout << "Match request: \'" << request.name << "\' with document id: " << request.id << endl;
            const auto [words, status] = search_server.MatchDocument(move(request.name), request.id);
            PrintMatchDocumentResult(request.id, words, status);
            break;
        }
        case RequestType::NO_RESULT:
        {
            cout << "Total empty requests: "s << request_queue.GetNoResultRequests() << endl;
            break;
        }
        }
    }
}

int main(int argc, const char** argv)
try {
    if (argc != 2) {
        cerr << "Usage: "sv << argv[0] << " <in_file>"sv << endl;
        _getch();
        return 1;
    }
    std::filesystem::path file = argv[1];
    Reader reader;
    reader.LoadXML(file.string());
    SearchServer search_server(move(reader.GetStopWords()));
    RequestQueue request_queue(search_server);
    AddDocuments(reader, search_server);
    PrintResults(reader, search_server, request_queue);
    _getch();
    return 0;
}

catch (const invalid_argument& e) {
    cout << "Error: "s << e.what() << endl;
    _getch();
}

catch (const out_of_range& e) {
    cout << "Error: "s << e.what() << endl;
    _getch();
}

catch (...) {
    cout << "Unknown exception!"s << endl;
    _getch();
}