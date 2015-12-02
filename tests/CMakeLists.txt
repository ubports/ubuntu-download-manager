# Copyright © 2014 Canonical Ltd.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License version 3 as
# published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
# Authored by: Manuel de la Peña <manuel.delapena@canonical.com>

if(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")
add_custom_target(tests-data ALL
        COMMAND cp -r ${CMAKE_SOURCE_DIR}/tests/data ${CMAKE_CURRENT_BINARY_DIR}/data
)
endif(NOT "${CMAKE_CURRENT_SOURCE_DIR}" STREQUAL "${CMAKE_CURRENT_BINARY_DIR}")

add_subdirectory(common)
add_subdirectory(downloads)
add_subdirectory(uploads)
add_subdirectory(abi-compliance)
