
cpp_test(sanity sanity.cpp)

cpp_test(gmm_gaussian gmm_gaussian.cpp)
target_link_libraries(test_gmm_gaussian PRIVATE grabcut Eigen3::Eigen)

cpp_test(gmm_mean_covar_precompute test_gmm_mv_precompute.cpp)
target_link_libraries(test_gmm_mean_covar_precompute PRIVATE grabcut Eigen3::Eigen)

cpp_test(quantize test_quantize.cpp)
target_link_libraries(test_quantize PRIVATE grabcut Eigen3::Eigen stb_utils)