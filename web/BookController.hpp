#pragma once
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/web/server/api/ApiController.hpp"
#include "BookDto.hpp"
#include "BookService.hpp"
#include <fstream>
#include <string>

using namespace std;

#include OATPP_CODEGEN_BEGIN(ApiController)

class BookController : public oatpp::web::server::api::ApiController {
private:
  BookService m_service;
public:
  BookController(const std::shared_ptr<ObjectMapper>& objectMapper)
    : ApiController(objectMapper) {}

  ENDPOINT("GET", "/books", listBooks) {
    auto arr = oatpp::Vector<Object<BookDto>>::createShared();
    for (auto& b : m_service.listAll()) {
      auto dto = BookDto::createShared();
      dto->id = b.id.c_str();
      dto->title = b.title.c_str();
      dto->author = b.author.c_str();
      dto->publisher = b.publisher.c_str();
      dto->price = b.price;
      dto->totalCopies = b.totalCopies;
      dto->availableCopies = b.availableCopies;
      arr->push_back(dto);
    }
    return createDtoResponse(Status::CODE_200, arr);
  }

  ENDPOINT("GET", "/", index) {
    ifstream in("webroot/index.html", ios::binary);
    if (!in) return createResponse(Status::CODE_404, "index.html not found");
    string html((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    auto resp = ResponseFactory::createResponse(Status::CODE_200, html.c_str());
    resp->putHeader("Content-Type", "text/html; charset=utf-8");
    return resp;
  }

  ENDPOINT("GET", "/style.css", style) {
    ifstream in("webroot/style.css", ios::binary);
    if (!in) return createResponse(Status::CODE_404, "style.css not found");
    string css((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    auto resp = ResponseFactory::createResponse(Status::CODE_200, css.c_str());
    resp->putHeader("Content-Type", "text/css; charset=utf-8");
    return resp;
  }

  ENDPOINT("GET", "/app.js", script) {
    ifstream in("webroot/app.js", ios::binary);
    if (!in) return createResponse(Status::CODE_404, "app.js not found");
    string js((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
    auto resp = ResponseFactory::createResponse(Status::CODE_200, js.c_str());
    resp->putHeader("Content-Type", "application/javascript; charset=utf-8");
    return resp;
  }

  ENDPOINT("GET", "/books/{id}", getBook, PATH(String, id)) {
    auto rec = m_service.getById(id);
    if (!rec) return createResponse(Status::CODE_404, "Not Found");
    auto dto = BookDto::createShared();
    dto->id = rec->id.c_str();
    dto->title = rec->title.c_str();
    dto->author = rec->author.c_str();
    dto->publisher = rec->publisher.c_str();
    dto->price = rec->price;
    dto->totalCopies = rec->totalCopies;
    dto->availableCopies = rec->availableCopies;
    return createDtoResponse(Status::CODE_200, dto);
  }

  ENDPOINT("POST", "/books", createBook,
           BODY_DTO(Object<BookDto>, dto)) {
    Book b;
    b.id = dto->id ? string(dto->id->c_str()) : "";
    b.title = dto->title ? string(dto->title->c_str()) : "";
    b.author = dto->author ? string(dto->author->c_str()) : "";
    b.publisher = dto->publisher ? string(dto->publisher->c_str()) : "";
    b.price = dto->price;
    b.totalCopies = dto->totalCopies;
    b.availableCopies = dto->availableCopies;
    m_service.addOrAcquire(b);
    return createResponse(Status::CODE_201, "Created");
  }

  ENDPOINT("DELETE", "/books/{id}", deleteBook, PATH(String, id)) {
    if (m_service.removeById(id)) return createResponse(Status::CODE_200, "Deleted");
    return createResponse(Status::CODE_404, "Not Found");
  }

  ENDPOINT("PUT", "/books/{id}/borrow", borrow, PATH(String, id)) {
    if (m_service.borrow(id)) return createResponse(Status::CODE_200, "OK");
    return createResponse(Status::CODE_400, "Fail");
  }

  ENDPOINT("PUT", "/books/{id}/return", giveback, PATH(String, id)) {
    if (m_service.giveback(id)) return createResponse(Status::CODE_200, "OK");
    return createResponse(Status::CODE_400, "Fail");
  }

  ENDPOINT("GET", "/tree", getTree) {
    auto str = m_service.getTreeStructure();
    auto resp = ResponseFactory::createResponse(Status::CODE_200, str.c_str());
    resp->putHeader("Content-Type", "text/plain; charset=utf-8");
    return resp;
  }

  // 兜底路由：处理所有未匹配的 GET 请求
  // 1. 解决 IDE 预览时 URL 带参数导致 404 的问题
  // 2. 允许直接访问 /index.html
  // 3. 处理真正的 404
  ENDPOINT("GET", "*", wildcard, REQUEST(std::shared_ptr<IncomingRequest>, request)) {
    auto path = request->getPathTail();
    // 如果是根路径的变体（由 IDE 产生）或者显式请求 index.html，都返回首页
    if (path == nullptr || path->empty() || 
        path->find("ide_webview_request_time") != string::npos || 
        path->compare("index.html") == 0) {
      ifstream in("webroot/index.html", ios::binary);
      if (!in) return createResponse(Status::CODE_404, "index.html not found");
      string html((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
      auto resp = ResponseFactory::createResponse(Status::CODE_200, html.c_str());
      resp->putHeader("Content-Type", "text/html; charset=utf-8");
      return resp;
    }
    return createResponse(Status::CODE_404, "Page not found");
  }
};

#include OATPP_CODEGEN_END(ApiController)
