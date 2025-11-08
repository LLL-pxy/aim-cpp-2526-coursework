#include "Song.h"
#include <iostream>
#include <algorithm>
#include <cctype>
#include <sstream>

// 匿名命名空间，用于存放仅在本文件内使用的辅助函数
namespace {

/**
 * @brief 返回去除首尾空白字符的字符串副本。
 * @param s 原始字符串。
 * @return 去除首尾空白后的新字符串。
 */
std::string trim_copy(const std::string &s) {
    if (s.empty()) return s;

    auto start_it = s.begin();
    while (start_it != s.end() && std::isspace(static_cast<unsigned char>(*start_it))) {
        ++start_it;
    }

    auto end_it = s.end() - 1;
    while (end_it >= start_it && std::isspace(static_cast<unsigned char>(*end_it))) {
        --end_it;
    }

    return std::string(start_it, end_it + 1);
}

/**
 * @brief 返回字符串的小写副本。
 * @param s 原始字符串。
 * @return 全小写的新字符串。
 */
std::string to_lower_copy(const std::string &s) {
    std::string lower_str;
    lower_str.reserve(s.size());
    for (unsigned char c : s) {
        lower_str.push_back(static_cast<char>(std::tolower(c)));
    }
    return lower_str;
}

/**
 * @brief 将标签向量连接成一个逗号分隔的字符串。
 * @param tags 标签向量。
 * @return 格式化后的标签字符串，如 "rock, jp, live"。
 */
std::string join_tags(const std::vector<std::string> &tags) {
    if (tags.empty()) return "";

    std::ostringstream oss;
    oss << tags[0];
    for (size_t i = 1; i < tags.size(); ++i) {
        oss << ", " << tags[i];
    }
    return oss.str();
}

} // 匿名命名空间结束
// 初始化 Song 类的静态成员变量 next_id_
int Song::next_id_ = 1;
Song::Song(const std::string &title,
           const std::string &artist,
           int duration_sec,
           int rating) {
    
    std::string trimmed_title = trim_copy(title);
    std::string trimmed_artist = trim_copy(artist);

    // 按照头文件提示的顺序进行校验
    if (trimmed_title.empty()) {
        std::cout << "[错误] 标题不能为空\n";
        return;
    }
    if (trimmed_artist.empty()) {
        std::cout << "[错误] 艺人不能为空\n";
        return;
    }
    if (duration_sec <= 0) {
        std::cout << "[错误] 时长必须为正整数（秒）\n";
        return;
    }
    if (rating < 1 || rating > 5) {
        std::cout << "[错误] 评分必须在 1...5 之间\n";
        return;
    }

    // 所有校验通过，初始化对象
    id_ = next_id_++;
    title_ = trimmed_title;
    artist_ = trimmed_artist;
    duration_sec_ = duration_sec;
    rating_ = rating;
    valid_ = true;
}
bool Song::set_title(const std::string &t) {
    std::string trimmed_t = trim_copy(t);
    if (trimmed_t.empty()) {
        std::cout << "[提示] 标题不能为空，已忽略本次修改\n";
        return false;
    }
    title_ = trimmed_t;
    return true;
}

bool Song::set_artist(const std::string &a) {
    std::string trimmed_a = trim_copy(a);
    if (trimmed_a.empty()) {
        std::cout << "[提示] 艺人不能为空，已忽略本次修改\n";
        return false;
    }
    artist_ = trimmed_a;
    return true;
}

bool Song::set_duration(int sec) {
    if (sec <= 0) {
        std::cout << "[提示] 时长需为正整数，已忽略本次修改\n";
        return false;
    }
    duration_sec_ = sec;
    return true;
}

bool Song::set_rating(int r) {
    if (r < 1 || r > 5) {
        std::cout << "[提示] 评分需在 1..5，已忽略本次修改\n";
        return false;
    }
    rating_ = r;
    return true;
}
bool Song::add_tag(const std::string &tag) {
    std::string trimmed_tag = trim_copy(tag);
    if (trimmed_tag.empty()) {
        std::cout << "[提示] 空标签已忽略\n";
        return false;
    }

    std::string lower_tag = to_lower_copy(trimmed_tag);
    for (const std::string &existing_tag : tags_) {
        if (to_lower_copy(existing_tag) == lower_tag) {
            std::cout << "[提示] 标签已存在（忽略大小写）\n";
            return false;
        }
    }

    tags_.push_back(trimmed_tag);
    return true;
}

bool Song::remove_tag(const std::string &tag) {
    std::string trimmed_tag = trim_copy(tag);
    std::string lower_tag = to_lower_copy(trimmed_tag);

    for (size_t i = 0; i < tags_.size(); ++i) {
        if (to_lower_copy(tags_[i]) == lower_tag) {
            tags_.erase(tags_.begin() + i);
            return true;
        }
    }

    std::cout << "[提示] 未找到该标签\n";
    return false;
}
bool Song::matches_keyword(const std::string &kw) const {
    std::string lower_kw = to_lower_copy(trim_copy(kw));
    if (lower_kw.empty()) {
        return false;
    }

    if (to_lower_copy(title_).find(lower_kw) != std::string::npos) {
        return true;
    }
    if (to_lower_copy(artist_).find(lower_kw) != std::string::npos) {
        return true;
    }
    for (const std::string &tag : tags_) {
        if (to_lower_copy(tag).find(lower_kw) != std::string::npos) {
            return true;
        }
    }

    return false;
}
std::ostream &operator<<(std::ostream &os, const Song &s) {
    os << "[#" << s.id_ << "] " << s.artist_ << " - " << s.title_ 
       << " (" << s.duration_sec_ << "s) " << std::string(s.rating_, '*');
    
    if (!s.tags_.empty()) {
        // Note: two spaces before [tags: ...] to match expected output formatting
        os << "  [tags: " << join_tags(s.tags_) << "]";
    }
    
    return os;
}

bool operator<(const Song &a, const Song &b) {
    if (a.rating_ != b.rating_) {
        return a.rating_ > b.rating_; // 评分高的在前
    }
    if (a.title_ != b.title_) {
        return a.title_ < b.title_;   // 标题字典序在前
    }
    return a.id_ < b.id_;             // ID小的在前
}