#pragma once
#include "oatpp/core/macro/codegen.hpp"
#include "oatpp/core/Types.hpp"

#include OATPP_CODEGEN_BEGIN(DTO)

class BookDto : public oatpp::DTO {
  DTO_INIT(BookDto, DTO)
  DTO_FIELD(String, id);
  DTO_FIELD(String, title);
  DTO_FIELD(String, author);
  DTO_FIELD(String, publisher);
  DTO_FIELD(Float64, price);
  DTO_FIELD(Int32, totalCopies);
  DTO_FIELD(Int32, availableCopies);
};

#include OATPP_CODEGEN_END(DTO)

