#pragma once
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

// 借阅记录DTO
class BorrowRecordDto : public oatpp::DTO {
  DTO_INIT(BorrowRecordDto, DTO)
  DTO_FIELD(String, borrowerId);      // 借阅者图书证号
  DTO_FIELD(String, borrowerName);    // 借阅者姓名
  DTO_FIELD(Int64, borrowTime);       // 借阅时间（Unix时间戳）
  DTO_FIELD(Int64, expectedReturnTime); // 预期归还时间（Unix时间戳）
  DTO_FIELD(Int64, actualReturnTime);   // 实际归还时间（0表示未归还）
};

// 预约记录DTO
class ReservationRecordDto : public oatpp::DTO {
  DTO_INIT(ReservationRecordDto, DTO)
  DTO_FIELD(String, reserverId);      // 预约者图书证号
  DTO_FIELD(String, reserverName);    // 预约者姓名
  DTO_FIELD(Int64, reserveTime);      // 预约时间（Unix时间戳）
  DTO_FIELD(Int64, expectedPickupTime); // 预期取书时间（Unix时间戳）
  DTO_FIELD(Int64, actualPickupTime);   // 实际取书时间（0表示未取书）
};

// 图书DTO
class BookDto : public oatpp::DTO {
  DTO_INIT(BookDto, DTO)
  DTO_FIELD(String, id);              // 书号
  DTO_FIELD(String, title);           // 书名
  DTO_FIELD(String, author);          // 作者
  DTO_FIELD(String, publisher);       // 出版社
  DTO_FIELD(Float64, price);          // 定价
  DTO_FIELD(String, publishDate);     // 出版时间
  DTO_FIELD(Int32, totalCopies);      // 总存量
  DTO_FIELD(Int32, availableCopies);  // 现存量
  DTO_FIELD(Vector<Object<BorrowRecordDto>>, borrowRecords);      // 借阅记录列表
  DTO_FIELD(Vector<Object<ReservationRecordDto>>, reservationRecords);  // 预约记录列表
};

// 借阅请求DTO
class BorrowRequestDto : public oatpp::DTO {
  DTO_INIT(BorrowRequestDto, DTO)
  DTO_FIELD(String, borrowerId);      // 借阅者图书证号
  DTO_FIELD(String, borrowerName);    // 借阅者姓名
};

// 归还请求DTO
class ReturnRequestDto : public oatpp::DTO {
  DTO_INIT(ReturnRequestDto, DTO)
  DTO_FIELD(String, borrowerId);      // 借阅者图书证号
};

// 预约请求DTO
class ReserveRequestDto : public oatpp::DTO {
  DTO_INIT(ReserveRequestDto, DTO)
  DTO_FIELD(String, reserverId);      // 预约者图书证号
  DTO_FIELD(String, reserverName);    // 预约者姓名
};

#include OATPP_CODEGEN_END(DTO)

