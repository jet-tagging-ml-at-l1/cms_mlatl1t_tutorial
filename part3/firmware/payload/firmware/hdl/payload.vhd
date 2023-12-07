library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

use work.ipbus.all;
use work.emp_data_types.all;
use work.emp_project_decl.all;

use work.emp_device_decl.all;
use work.emp_ttc_decl.all;
use work.emp_slink_types.all;

entity emp_payload is
  port(
    clk         : in  std_logic;        -- ipbus signals
    rst         : in  std_logic;
    ipb_in      : in  ipb_wbus;
    ipb_out     : out ipb_rbus;
    clk40       : in  std_logic;	
    clk_payload : in  std_logic_vector(2 downto 0);
    rst_payload : in  std_logic_vector(2 downto 0);
    clk_p       : in  std_logic;        -- data clock
    rst_loc     : in  std_logic_vector(N_REGION - 1 downto 0);
    clken_loc   : in  std_logic_vector(N_REGION - 1 downto 0);
    ctrs        : in  ttc_stuff_array;
    bc0         : out std_logic;
    d           : in  ldata(4 * N_REGION - 1 downto 0);  -- data in
    q           : out ldata(4 * N_REGION - 1 downto 0);  -- data out
    gpio        : out std_logic_vector(29 downto 0);  -- IO to mezzanine connector
    gpio_en     : out std_logic_vector(29 downto 0);  -- IO to mezzanine connector (three-state enables)
    slink_q : out slink_input_data_quad_array(SLINK_MAX_QUADS-1 downto 0);
    backpressure : in std_logic_vector(SLINK_MAX_QUADS-1 downto 0)
    );
end emp_payload;

architecture rtl of emp_payload is

  signal X_scaled     : std_logic_vector(895 downto 0) := (others => '0');
  signal X_scaled_vld : std_logic := '0';
  signal y            : std_logic_vector(12 downto 0) := (others => '0');
  signal y_vld        : std_logic := '0';

begin

  -- scale the inputs
  ScalerInstance : work.ScalerWrapper
  port map(
    clk          => clk_p,
    d            => d_delayed,
    X_scaled     => X_scaled,
    X_scaled_vld => X_scaled_vld
  );

  -- run the NN
  NNInstance : work.NNWrapper
  port map(
    clk      => clk_p,
    X_scaled => X_scaled,
    X_vld    => X_scaled_vld,
    y        => y,
    y_vld    => y_vld
  );

  -- hook up the output
  q(0).data(12 downto 0) <= y;
  q(0).valid             <= y_vld;

	ipb_out <= IPB_RBUS_NULL;
	bc0 <= '0';
	gpio <= (others => '0');
	gpio_en <= (others => '0');

end rtl;