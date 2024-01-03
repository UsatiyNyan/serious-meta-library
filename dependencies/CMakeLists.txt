cpmaddpackage(
        NAME function2
        GIT_REPOSITORY "git@github.com:Naios/function2.git"
        GIT_TAG 4.2.4)

include("${CMAKE_CURRENT_SOURCE_DIR}/tl.cmake")

sl_target_link_system_libraries(${PROJECT_NAME}
        INTERFACE
        function2::function2
        tl::optional
)
