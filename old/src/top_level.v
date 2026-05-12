module top_level(
    input         CLOCK_50,
    input         reset_n,

    output [14:0] HPS_DDR3_ADDR,
    output [2:0]  HPS_DDR3_BA,
    output        HPS_DDR3_CK_P,
    output        HPS_DDR3_CK_N,
    output        HPS_DDR3_CKE,
    output        HPS_DDR3_CS_N,
    output        HPS_DDR3_RAS_N,
    output        HPS_DDR3_CAS_N,
    output        HPS_DDR3_WE_N,
    output        HPS_DDR3_RESET_N,
    inout  [31:0] HPS_DDR3_DQ,
    inout  [3:0]  HPS_DDR3_DQS_P,
    inout  [3:0]  HPS_DDR3_DQS_N,
    output        HPS_DDR3_ODT,
    output [3:0]  HPS_DDR3_DM,
    input         HPS_DDR3_RZQ
);

    wire        elm_clr;
    wire        elm_rst;
    wire        elm_enb;
    wire [31:0] elm_data_in;
    wire [31:0] elm_data_out;

    mogger u_soc(
        .clk_clk            (CLOCK_50),
        .reset_reset_n      (reset_n),
        .memory_mem_a       (HPS_DDR3_ADDR),
        .memory_mem_ba      (HPS_DDR3_BA),
        .memory_mem_ck      (HPS_DDR3_CK_P),
        .memory_mem_ck_n    (HPS_DDR3_CK_N),
        .memory_mem_cke     (HPS_DDR3_CKE),
        .memory_mem_cs_n    (HPS_DDR3_CS_N),
        .memory_mem_ras_n   (HPS_DDR3_RAS_N),
        .memory_mem_cas_n   (HPS_DDR3_CAS_N),
        .memory_mem_we_n    (HPS_DDR3_WE_N),
        .memory_mem_reset_n (HPS_DDR3_RESET_N),
        .memory_mem_dq      (HPS_DDR3_DQ),
        .memory_mem_dqs     (HPS_DDR3_DQS_P),
        .memory_mem_dqs_n   (HPS_DDR3_DQS_N),
        .memory_mem_odt     (HPS_DDR3_ODT),
        .memory_mem_dm      (HPS_DDR3_DM),
        .memory_oct_rzqin   (HPS_DDR3_RZQ),
        .conduit_clr        (elm_clr),
        .conduit_rst        (elm_rst),
        .conduit_enb        (elm_enb),
        .conduit_data_in    (elm_data_in),
        .conduit_data_out   (elm_data_out)
    );

    CoProcessor u_elm(
        .clk           (CLOCK_50),
        .rst           (elm_rst),
        .clr_operation (elm_clr),
        .enable        (elm_enb),
        .data_in       (elm_data_in),
        .data_out      (elm_data_out)
    );

endmodule