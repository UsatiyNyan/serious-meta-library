cpmaddpackage(
        NAME function2
        GIT_REPOSITORY "git@github.com:Naios/function2.git"
        GIT_TAG 4.2.4)

sl_target_link_system_libraries(${PROJECT_NAME}
        INTERFACE
        function2::function2
)
