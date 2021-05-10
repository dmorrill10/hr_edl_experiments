add_executable(run_fixed_ltbr run_fixed_ltbr.cc ${OPEN_SPIEL_OBJECTS})
target_link_libraries(run_fixed_ltbr absl::flags absl::strings absl::flags_parse ${ABSL})

add_executable(run_simultaneous_ltbr run_simultaneous_ltbr.cc ${OPEN_SPIEL_OBJECTS})
target_link_libraries(run_simultaneous_ltbr absl::flags absl::strings absl::flags_parse ${ABSL})

add_executable(run_corr_dist run_corr_dist.cc ${OPEN_SPIEL_OBJECTS})
target_link_libraries(run_corr_dist absl::flags absl::strings absl::flags_parse ${ABSL})

add_executable(show_max_and_min_utilities show_max_and_min_utilities.cc ${OPEN_SPIEL_OBJECTS})
target_link_libraries(show_max_and_min_utilities absl::flags absl::strings absl::flags_parse ${ABSL})

add_executable(show_game_size show_game_size.cc ${OPEN_SPIEL_OBJECTS})
target_link_libraries(show_game_size absl::flags absl::strings absl::flags_parse ${ABSL})
