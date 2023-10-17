# Content of generate_build_header.cmake
# Call the custom function to generate the PicoGKBuild.h file

# Function to generate the PicoGKBuild.h file
function(generate_build_header)
    # Get the current date and time
    string(TIMESTAMP BUILD_DATE_TIME "%Y-%m-%d %H:%M:%S")

    # Create the content of the PicoGKBuild.h file
    set(    BUILD_HEADER_CONTENT
"#ifndef PICOGK_BUILD_H\n#define PICOGK_BUILD_H\n\n\
#define PICOGK_BUILD       \"${BUILD_DATE_TIME}\"\n\
#define PICOGK_LIB_VERSION \"${LIB_VERSION}\"\n\
#define PICOGK_LIB_NAME    \"${LIB_NAME}\"\n
#endif // PICOGK_BUILD_H\n"
        )

    file(WRITE "${APIDIR}/PicoGKBuild.h" "${BUILD_HEADER_CONTENT}")
endfunction()

generate_build_header()
