#include "../include/Frame.h"
#include "../include/Utils.h"
#include <sstream>

Frame::Frame()
    : type(""),
      headers(),
      body("") { }

Frame::Frame(std::string msg)
    : type(""),
      headers(),
      body("") {
    std::vector<std::string> lines = Utils::splitNoEmpty(msg, '\n');
    if (lines.empty())
        return;
    type = lines[0];
    size_t i = 1;
    while (i < lines.size() && !lines[i].empty()) {
        std::vector<std::string> parts = Utils::splitNoEmpty(lines[i], ':');
        if (parts.size() >= 2)
            headers[parts[0]] = parts[1];
        i++;
    }

    i++;
    while (i < lines.size()) {
        body += lines[i];
        if (i < lines.size() - 1)
            body += "\n";
        i++;
    }
}

void Frame::addHeader(std::string key, std::string value) { headers[key] = value; }

void Frame::setType(std::string newType) { type = newType; }

void Frame::setBody(std::string newBody) { body = newBody; }

std::string Frame::getHeaderValue(std::string key) {
    if (headers.count(key)) {
        return headers.at(key);
    }
    return "";
}

std::string Frame::getType() { return type; }

std::string Frame::getBody() { return body; }

std::string Frame::toString() {
    std::stringstream ss;
    ss << type << "\n";
    for (const auto& pair : headers) {
        ss << pair.first << ":" << pair.second << "\n";
    }
    ss << "\n";
    ss << body;
    ss << '\0';
    return ss.str();
}