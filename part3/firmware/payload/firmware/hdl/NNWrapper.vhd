library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

library NNLib;

use work.emp_data_types.all;
use work.emp_project_decl.all;

entity NNWrapper is
port(
  clk      : in std_logic;
  X_scaled : in std_logic_vector(895 downto 0);
  X_vld    : in std_logic;
  y        : out std_logic_vector(12 downto 0);
  y_vld    : out std_logic
);
end NNWrapper;

architecture rtl of NNWrapper is

begin 

  -- make an instance of the Neural Network
  NNInstance : NNLib.L1TMLDemo_v1
  port map(
    ap_clk => clk,
    ap_rst => '0', -- never reset
    ap_start => '1', -- always run
    input_1_V_ap_vld => X_scaled,
    input_1_V => X_slv,
    layer13_out_0_V => y,
    layer13_out_0_V_ap_vld => y_vld
  );

end rtl;