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
    size_t bodyDelimiter = msg.find("\n\n");
    std::string headerSection;

    if (bodyDelimiter == std::string::npos) {
        headerSection = msg;
    } else {
        headerSection = msg.substr(0, bodyDelimiter);
        body = msg.substr(bodyDelimiter + 2);
    }

    std::vector<std::string> lines = Utils::splitNoEmpty(headerSection, '\n');
    if (lines.empty())
        return;

    type = lines[0];

    for (size_t i = 1; i < lines.size(); i++) {
        size_t colonPos = lines[i].find(':');
        if (colonPos != std::string::npos) {
            std::string key = lines[i].substr(0, colonPos);
            std::string value = lines[i].substr(colonPos + 1);
            headers[key] = value;
        }
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