cmake_minimum_required(VERSION 3.20)

project(Engine)

# Very good for debugging memory leaks
# add_compile_options(-fsanitize=address)
# add_link_options(-fsanitize=address)

add_subdirectory(RealEngine)
add_subdirectory(Voxel-Tracer)
