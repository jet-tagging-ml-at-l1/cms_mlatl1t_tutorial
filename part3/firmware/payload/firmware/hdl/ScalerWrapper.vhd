library IEEE;
use IEEE.std_logic_1164.all;
use IEEE.numeric_std.all;

library ScalerLib;

use work.emp_data_types.all;
use work.emp_project_decl.all;
use work.emp_device_decl.all;

entity ScalerWrapper is
port(
  clk      : in std_logic;
  d        : in  ldata(4 * N_REGION - 1 downto 0);  -- data in
  X_scaled : out std_logic_vector(895 downto 0);
  X_vld    : out std_logic
);
end ScalerWrapper;

architecture rtl of ScalerWrapper is

begin 

  -- make an instance of the Standard Scaler
  ScalerInstance : entity ScalerLib.scaler
  port map(
    ap_clk => clk,
    ap_rst => '0', -- never reset
    ap_start => d(0).valid, -- run whenever the MET link is valid
    -- MET
    X_unscaled_0_V => d(0).data(13 downto 0),
    X_unscaled_1_V => d(0).data(27 downto 14),

    X_unscaled_2_V => d(1).data(13 downto 0),
    X_unscaled_3_V => d(1).data(27 downto 14),
    X_unscaled_4_V => d(1).data(41 downto 28),

    X_unscaled_5_V => d(2).data(13 downto 0),
    X_unscaled_6_V => d(2).data(27 downto 14),
    X_unscaled_7_V => d(2).data(41 downto 28),

    X_unscaled_8_V => d(3).data(13 downto 0),
    X_unscaled_9_V => d(3).data(27 downto 14),
    X_unscaled_10_V => d(3).data(41 downto 28),

    X_unscaled_11_V => d(4).data(13 downto 0),
    X_unscaled_12_V => d(4).data(27 downto 14),
    X_unscaled_13_V => d(4).data(41 downto 28),

    X_unscaled_14_V => d(5).data(13 downto 0),
    X_unscaled_15_V => d(5).data(27 downto 14),
    X_unscaled_16_V => d(5).data(41 downto 28),

    X_unscaled_17_V => d(6).data(13 downto 0),
    X_unscaled_18_V => d(6).data(27 downto 14),
    X_unscaled_19_V => d(6).data(41 downto 28),

    X_unscaled_20_V => d(7).data(13 downto 0),
    X_unscaled_21_V => d(7).data(27 downto 14),
    X_unscaled_22_V => d(7).data(41 downto 28),

    X_unscaled_23_V => d(8).data(13 downto 0),
    X_unscaled_24_V => d(8).data(27 downto 14),
    X_unscaled_25_V => d(8).data(41 downto 28),

    X_unscaled_26_V => d(9).data(13 downto 0),
    X_unscaled_27_V => d(9).data(27 downto 14),
    X_unscaled_28_V => d(9).data(41 downto 28),

    X_unscaled_29_V => d(10).data(13 downto 0),
    X_unscaled_30_V => d(10).data(27 downto 14),
    X_unscaled_31_V => d(10).data(41 downto 28),

    X_unscaled_32_V => d(11).data(13 downto 0),
    X_unscaled_33_V => d(11).data(27 downto 14),
    X_unscaled_34_V => d(11).data(41 downto 28),

    X_unscaled_35_V => d(12).data(13 downto 0),
    X_unscaled_36_V => d(12).data(27 downto 14),
    X_unscaled_37_V => d(12).data(41 downto 28),

    X_unscaled_38_V => d(13).data(13 downto 0),
    X_unscaled_39_V => d(13).data(27 downto 14),
    X_unscaled_40_V => d(13).data(41 downto 28),

    X_unscaled_41_V => d(14).data(13 downto 0),
    X_unscaled_42_V => d(14).data(27 downto 14),
    X_unscaled_43_V => d(14).data(41 downto 28),

    X_unscaled_44_V => d(15).data(13 downto 0),
    X_unscaled_45_V => d(15).data(27 downto 14),
    X_unscaled_46_V => d(15).data(41 downto 28),

    X_unscaled_47_V => d(112).data(13 downto 0),
    X_unscaled_48_V => d(112).data(27 downto 14),
    X_unscaled_49_V => d(112).data(41 downto 28),

    X_unscaled_50_V => d(113).data(13 downto 0),
    X_unscaled_51_V => d(113).data(27 downto 14),
    X_unscaled_52_V => d(113).data(41 downto 28),

    X_unscaled_53_V => d(114).data(13 downto 0),
    X_unscaled_54_V => d(114).data(27 downto 14),
    X_unscaled_55_V => d(114).data(41 downto 28),

    X_scaled_V => X_scaled,
    X_scaled_V_ap_vld => X_vld
  );

end rtl;