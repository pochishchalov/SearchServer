#pragma once

#include <ostream>
#include <iterator>
#include <vector>

template <typename Iterator>
class IteratorRange {
    Iterator begin_;
    Iterator end_;
    size_t size_;
public:
    IteratorRange(Iterator begin, Iterator end)
        :begin_(begin), end_(end), size_(distance(begin, end))
    {
    }
    auto begin() const { return begin_; };
    auto end() const { return end_; };
    size_t size() { return size; };
};

template <typename Iterator>
std::ostream& operator<<(std::ostream& os, const IteratorRange<Iterator>& Ir) {
    for (auto iter = Ir.begin(); iter != Ir.end(); ++iter) {
        os << *iter;
    }
    return os;
}

template <typename Iterator>
class Paginator {

public:

    Paginator(Iterator begin_range, Iterator end_range, size_t page_size) {
        AddPages(begin_range, end_range, page_size);
    }

    auto begin() const { return pages_.begin(); };
    auto end() const { return pages_.end(); };

    void AddPages(Iterator begin_range, Iterator end_range, size_t page_size) {
        if (page_size < static_cast<unsigned long long>(std::distance(begin_range, end_range))) {
            auto end_page = begin_range;
            std::advance(end_page, page_size);
            pages_.push_back({ begin_range, end_page });
            AddPages(end_page, end_range, page_size);
        }
        else {
            pages_.push_back({ begin_range, end_range });
        }
    }

private:

    std::vector<IteratorRange<Iterator>> pages_;
};

template <typename Iterator>
std::ostream& operator<<(std::ostream& os, const Paginator<Iterator>& paginator) {
    for (auto iter = paginator.begin(); iter != paginator.end(); ++iter) {
        os << *iter << ' ';
    }
    return os;
}

template <typename Container>
auto Paginate(const Container& c, size_t page_size) {
    return Paginator(begin(c), end(c), page_size);
}