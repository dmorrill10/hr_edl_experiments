add_library(hr_edl OBJECT
  samplers.cc
  spiel_extra.cc
  policy_evaluation.cc
  decision_point.cc
  best_response.cc
  math.cc
)

add_executable(policy_evaluation_test policy_evaluation_test.cc ${OPEN_SPIEL_OBJECTS})
target_link_libraries(policy_evaluation_test ${ABSL})
add_test(policy_evaluation_test policy_evaluation_test)

add_executable(decision_point_test decision_point_test.cc ${OPEN_SPIEL_OBJECTS})
target_link_libraries(decision_point_test ${ABSL})
add_test(decision_point_test decision_point_test)

add_executable(osp_best_response_test best_response_test.cc ${OPEN_SPIEL_OBJECTS})
target_link_libraries(osp_best_response_test ${ABSL})
add_test(osp_best_response_test osp_best_response_test)

add_executable(action_transformation_test action_transformation_test.cc ${OPEN_SPIEL_OBJECTS})
target_link_libraries(action_transformation_test ${ABSL})
add_test(action_transformation_test action_transformation_test)
