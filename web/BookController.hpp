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
      arr->push_back(bookToDto(&b));
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
    auto dto = bookToDto(rec);
    return createDtoResponse(Status::CODE_200, dto);
  }

  ENDPOINT("GET", "/books/author/{author}", getBooksByAuthor, PATH(String, author)) {
    auto books = m_service.getByAuthor(author);
    auto arr = oatpp::Vector<Object<BookDto>>::createShared();
    for (auto& b : books) {
      arr->push_back(bookToDto(&b));
    }
    return createDtoResponse(Status::CODE_200, arr);
  }

  ENDPOINT("POST", "/books", createBook,
    auto dto = BookDto::createShared();
    if (!rec) return dto;
    
    dto->id = rec->id.c_str();
    dto->title = rec->title.c_str();
    dto->author = rec->author.c_str();
    dto->publisher = rec->publisher.c_str();
    dto->price = rec->price;
    dto->publishDate = rec->publishDate.c_str();
    dto->totalCopies = rec->totalCopies;
    dto->availableCopies = rec->availableCopies;
    
    // 转换借阅记录
    auto borrowRecords = oatpp::Vector<Object<BorrowRecordDto>>::createShared();
    for (const auto& record : rec->borrowRecords) {
      auto borrowDto = BorrowRecordDto::createShared();
      borrowDto->borrowerId = record.borrowerId.c_str();
      borrowDto->borrowerName = record.borrowerName.c_str();
      borrowDto->borrowTime = record.borrowTime;
      borrowDto->expectedReturnTime = record.expectedReturnTime;
      borrowDto->actualReturnTime = record.actualReturnTime;
      borrowRecords->push_back(borrowDto);
    }
    dto->borrowRecords = borrowRecords;
    
    // 转换预约记录
    auto reservationRecords = oatpp::Vector<Object<ReservationRecordDto>>::createShared();
    for (const auto& record : rec->reservationRecords) {
      auto reserveDto = ReservationRecordDto::createShared();
      reserveDto->reserverId = record.reserverId.c_str();
      reserveDto->reserverName = record.reserverName.c_str();
      reserveDto->reserveTime = record.reserveTime;
      reserveDto->expectedPickupTime = record.expectedPickupTime;
      reserveDto->actualPickupTime = record.actualPickupTime;
      reservationRecords->push_back(reserveDto);
    }
    dto->reservationRecords = reservationRecords;
    
    return dto;
  }

           BODY_DTO(Object<BookDto>, dto)) {
    Book b;
    b.id = dto->id ? string(dto->id->c_str()) : "";
    b.title = dto->title ? string(dto->title->c_str()) : "";
    b.author = dto->author ? string(dto->author->c_str()) : "";
    b.publisher = dto->publisher ? string(dto->publisher->c_str()) : "";
    b.price = dto->price;
    b.publishDate = dto->publishDate ? string(dto->publishDate->c_str()) : "";
    b.totalCopies = dto->totalCopies;
    b.availableCopies = dto->totalCopies;  // 新书初始可借数量等于总数量
    m_service.addOrAcquire(b);
    return createResponse(Status::CODE_201, "Created");
  }

  ENDPOINT("DELETE", "/books/{id}", deleteBook, PATH(String, id)) {
    if (m_service.removeById(id)) return createResponse(Status::CODE_200, "Deleted");
    return createResponse(Status::CODE_404, "Not Found");
  }

  ENDPOINT("PUT", "/books/{id}/borrow", borrow, 
           PATH(String, id),
           BODY_DTO(Object<BorrowRequestDto>, dto)) {
    if (!dto || !dto->borrowerId || !dto->borrowerName) {
      return createResponse(Status::CODE_400, "Missing borrowerId or borrowerName");
    }
    string borrowerId = dto->borrowerId->c_str();
    string borrowerName = dto->borrowerName->c_str();
    if (m_service.borrow(id, borrowerId, borrowerName)) {
      return createResponse(Status::CODE_200, "OK");
    }
    return createResponse(Status::CODE_400, "Borrow failed: book not available");
  }

  ENDPOINT("PUT", "/books/{id}/return", giveback, 
           PATH(String, id),
           BODY_DTO(Object<ReturnRequestDto>, dto)) {
    if (!dto || !dto->borrowerId) {
      return createResponse(Status::CODE_400, "Missing borrowerId");
    }
    string borrowerId = dto->borrowerId->c_str();
    if (m_service.giveback(id, borrowerId)) {
      return createResponse(Status::CODE_200, "OK");
    }
    return createResponse(Status::CODE_400, "Return failed");
  }

  ENDPOINT("PUT", "/books/{id}/reserve", reserve,
           PATH(String, id),
           BODY_DTO(Object<ReserveRequestDto>, dto)) {
    if (!dto || !dto->reserverId || !dto->reserverName) {
      return createResponse(Status::CODE_400, "Missing reserverId or reserverName");
    }
    string reserverId = dto->reserverId->c_str();
    string reserverName = dto->reserverName->c_str();
    if (m_service.reserve(id, reserverId, reserverName)) {
      return createResponse(Status::CODE_200, "OK");
    }
    return createResponse(Status::CODE_400, "Reserve failed: book available or already reserved");
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

private:
  // 将Book转换为BookDto的辅助函数
  Object<BookDto> bookToDto(Book* rec) {
    auto dto = BookDto::createShared();
    if (!rec) return dto;
    
    dto->id = rec->id.c_str();
    dto->title = rec->title.c_str();
    dto->author = rec->author.c_str();
    dto->publisher = rec->publisher.c_str();
    dto->price = rec->price;
    dto->publishDate = rec->publishDate.c_str();
    dto->totalCopies = rec->totalCopies;
    dto->availableCopies = rec->availableCopies;
    
    // 转换借阅记录
    auto borrowRecords = oatpp::Vector<Object<BorrowRecordDto>>::createShared();
    for (const auto& record : rec->borrowRecords) {
      auto borrowDto = BorrowRecordDto::createShared();
      borrowDto->borrowerId = record.borrowerId.c_str();
      borrowDto->borrowerName = record.borrowerName.c_str();
      borrowDto->borrowTime = record.borrowTime;
      borrowDto->expectedReturnTime = record.expectedReturnTime;
      borrowDto->actualReturnTime = record.actualReturnTime;
      borrowRecords->push_back(borrowDto);
    }
    dto->borrowRecords = borrowRecords;
    
    // 转换预约记录
    auto reservationRecords = oatpp::Vector<Object<ReservationRecordDto>>::createShared();
    for (const auto& record : rec->reservationRecords) {
      auto reserveDto = ReservationRecordDto::createShared();
      reserveDto->reserverId = record.reserverId.c_str();
      reserveDto->reserverName = record.reserverName.c_str();
      reserveDto->reserveTime = record.reserveTime;
      reserveDto->expectedPickupTime = record.expectedPickupTime;
      reserveDto->actualPickupTime = record.actualPickupTime;
      reservationRecords->push_back(reserveDto);
    }
    dto->reservationRecords = reservationRecords;
    
    return dto;
  }
};

#include OATPP_CODEGEN_END(ApiController)
