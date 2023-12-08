library IEEE;
use IEEE.STD_LOGIC_1164.all;

use work.emp_framework_decl.all;
use work.emp_device_types.all;

-------------------------------------------------------------------------------
package emp_project_decl is

  constant PAYLOAD_REV : std_logic_vector(31 downto 0) := X"70900000";

  constant LB_ADDR_WIDTH   : integer             := 10;
  -- Clock setup
  constant CLOCK_COMMON_RATIO : integer               := 36;
  constant CLOCK_RATIO        : integer               := 9;
  constant CLOCK_AUX_DIV      : clock_divisor_array_t := (18, 9, 4); -- Dividers of CLOCK_COMMON_RATIO * 40 MHz

  constant REGION_CONF : region_conf_array_t := (
    9      => (no_mgt, buf, no_fmt, buf, no_mgt),
    others => kDummyRegion
    );

end emp_project_decl;
