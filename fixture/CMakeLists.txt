add_library(${PROJECT_NAME}_fixture STATIC src/lifecycle.cpp)
target_include_directories(${PROJECT_NAME}_fixture PUBLIC include)
target_link_libraries(${PROJECT_NAME}_fixture PUBLIC ${PROJECT_NAME})
