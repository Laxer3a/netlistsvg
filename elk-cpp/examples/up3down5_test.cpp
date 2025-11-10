// Auto-generated C++ code for up3down5 graph
// Generated from netlistsvg + elkjs output
// 31 nodes, 40 edges

#include <elk/graph/graph.h>
#include <elk/alg/layered/layered_layout.h>
#include <iostream>
#include <memory>
#include <map>

using namespace elk;

int main() {
    auto root = std::make_unique<Node>("up3down5");

    std::map<std::string, Node*> nodeMap;

    // Create nodes
    auto node__D_add_D_input_P_v_C_17_D_3 = root->addChild("$add$input.v:17$3");
    node__D_add_D_input_P_v_C_17_D_3->size = Size(25, 25);
    nodeMap["$add$input.v:17$3"] = node__D_add_D_input_P_v_C_17_D_3;
    auto node__D_add_D_input_P_v_C_17_D_3_p0 = node__D_add_D_input_P_v_C_17_D_3->addPort("$add$input.v:17$3.A", PortSide::NORTH);
    node__D_add_D_input_P_v_C_17_D_3_p0->position = Point(2, 5);
    auto node__D_add_D_input_P_v_C_17_D_3_p1 = node__D_add_D_input_P_v_C_17_D_3->addPort("$add$input.v:17$3.B", PortSide::NORTH);
    node__D_add_D_input_P_v_C_17_D_3_p1->position = Point(2, 20);
    auto node__D_add_D_input_P_v_C_17_D_3_p2 = node__D_add_D_input_P_v_C_17_D_3->addPort("$add$input.v:17$3.Y", PortSide::EAST);
    node__D_add_D_input_P_v_C_17_D_3_p2->position = Point(26, 12.5);

    auto node__D_and_D_input_P_v_C_28_D_5 = root->addChild("$and$input.v:28$5");
    node__D_and_D_input_P_v_C_28_D_5->size = Size(30, 25);
    nodeMap["$and$input.v:28$5"] = node__D_and_D_input_P_v_C_28_D_5;
    auto node__D_and_D_input_P_v_C_28_D_5_p0 = node__D_and_D_input_P_v_C_28_D_5->addPort("$and$input.v:28$5.A", PortSide::WEST);
    node__D_and_D_input_P_v_C_28_D_5_p0->position = Point(0, 5);
    auto node__D_and_D_input_P_v_C_28_D_5_p1 = node__D_and_D_input_P_v_C_28_D_5->addPort("$and$input.v:28$5.B", PortSide::WEST);
    node__D_and_D_input_P_v_C_28_D_5_p1->position = Point(0, 20);
    auto node__D_and_D_input_P_v_C_28_D_5_p2 = node__D_and_D_input_P_v_C_28_D_5->addPort("$and$input.v:28$5.Y", PortSide::EAST);
    node__D_and_D_input_P_v_C_28_D_5_p2->position = Point(30, 12.5);

    auto node__D_and_D_input_P_v_C_29_D_6 = root->addChild("$and$input.v:29$6");
    node__D_and_D_input_P_v_C_29_D_6->size = Size(30, 25);
    nodeMap["$and$input.v:29$6"] = node__D_and_D_input_P_v_C_29_D_6;
    auto node__D_and_D_input_P_v_C_29_D_6_p0 = node__D_and_D_input_P_v_C_29_D_6->addPort("$and$input.v:29$6.A", PortSide::WEST);
    node__D_and_D_input_P_v_C_29_D_6_p0->position = Point(0, 5);
    auto node__D_and_D_input_P_v_C_29_D_6_p1 = node__D_and_D_input_P_v_C_29_D_6->addPort("$and$input.v:29$6.B", PortSide::WEST);
    node__D_and_D_input_P_v_C_29_D_6_p1->position = Point(0, 20);
    auto node__D_and_D_input_P_v_C_29_D_6_p2 = node__D_and_D_input_P_v_C_29_D_6->addPort("$and$input.v:29$6.Y", PortSide::EAST);
    node__D_and_D_input_P_v_C_29_D_6_p2->position = Point(30, 12.5);

    auto node__D_procdff_D_40 = root->addChild("$procdff$40");
    node__D_procdff_D_40->size = Size(34, 44);
    nodeMap["$procdff$40"] = node__D_procdff_D_40;
    auto node__D_procdff_D_40_p0 = node__D_procdff_D_40->addPort("$procdff$40.Q", PortSide::EAST);
    node__D_procdff_D_40_p0->position = Point(35, 10);
    auto node__D_procdff_D_40_p1 = node__D_procdff_D_40->addPort("$procdff$40.CLK", PortSide::NORTH);
    node__D_procdff_D_40_p1->position = Point(-1, 30);
    auto node__D_procdff_D_40_p2 = node__D_procdff_D_40->addPort("$procdff$40.C", PortSide::NORTH);
    node__D_procdff_D_40_p2->position = Point(-1, 30);
    auto node__D_procdff_D_40_p3 = node__D_procdff_D_40->addPort("$procdff$40.D", PortSide::NORTH);
    node__D_procdff_D_40_p3->position = Point(-1, 10);
    auto node__D_procdff_D_40_p4 = node__D_procdff_D_40->addPort("$procdff$40.ARST", PortSide::NORTH);
    node__D_procdff_D_40_p4->position = Point(17, 44);
    auto node__D_procdff_D_40_p5 = node__D_procdff_D_40->addPort("$procdff$40.SRST", PortSide::NORTH);
    node__D_procdff_D_40_p5->position = Point(17, 44);

    auto node__D_procdff_D_41 = root->addChild("$procdff$41");
    node__D_procdff_D_41->size = Size(30, 40);
    nodeMap["$procdff$41"] = node__D_procdff_D_41;
    auto node__D_procdff_D_41_p0 = node__D_procdff_D_41->addPort("$procdff$41.Q", PortSide::EAST);
    node__D_procdff_D_41_p0->position = Point(31, 10);
    auto node__D_procdff_D_41_p1 = node__D_procdff_D_41->addPort("$procdff$41.CLK", PortSide::NORTH);
    node__D_procdff_D_41_p1->position = Point(-1, 30);
    auto node__D_procdff_D_41_p2 = node__D_procdff_D_41->addPort("$procdff$41.C", PortSide::NORTH);
    node__D_procdff_D_41_p2->position = Point(-1, 30);
    auto node__D_procdff_D_41_p3 = node__D_procdff_D_41->addPort("$procdff$41.D", PortSide::NORTH);
    node__D_procdff_D_41_p3->position = Point(-1, 10);
    auto node__D_procdff_D_41_p4 = node__D_procdff_D_41->addPort("$procdff$41.ARST", PortSide::NORTH);
    node__D_procdff_D_41_p4->position = Point(15, 40);
    auto node__D_procdff_D_41_p5 = node__D_procdff_D_41->addPort("$procdff$41.SRST", PortSide::NORTH);
    node__D_procdff_D_41_p5->position = Point(15, 40);

    auto node__D_procdff_D_42 = root->addChild("$procdff$42");
    node__D_procdff_D_42->size = Size(30, 40);
    nodeMap["$procdff$42"] = node__D_procdff_D_42;
    auto node__D_procdff_D_42_p0 = node__D_procdff_D_42->addPort("$procdff$42.Q", PortSide::EAST);
    node__D_procdff_D_42_p0->position = Point(31, 10);
    auto node__D_procdff_D_42_p1 = node__D_procdff_D_42->addPort("$procdff$42.CLK", PortSide::NORTH);
    node__D_procdff_D_42_p1->position = Point(-1, 30);
    auto node__D_procdff_D_42_p2 = node__D_procdff_D_42->addPort("$procdff$42.C", PortSide::NORTH);
    node__D_procdff_D_42_p2->position = Point(-1, 30);
    auto node__D_procdff_D_42_p3 = node__D_procdff_D_42->addPort("$procdff$42.D", PortSide::NORTH);
    node__D_procdff_D_42_p3->position = Point(-1, 10);
    auto node__D_procdff_D_42_p4 = node__D_procdff_D_42->addPort("$procdff$42.ARST", PortSide::NORTH);
    node__D_procdff_D_42_p4->position = Point(15, 40);
    auto node__D_procdff_D_42_p5 = node__D_procdff_D_42->addPort("$procdff$42.SRST", PortSide::NORTH);
    node__D_procdff_D_42_p5->position = Point(15, 40);

    auto node__D_procdff_D_43 = root->addChild("$procdff$43");
    node__D_procdff_D_43->size = Size(30, 40);
    nodeMap["$procdff$43"] = node__D_procdff_D_43;
    auto node__D_procdff_D_43_p0 = node__D_procdff_D_43->addPort("$procdff$43.Q", PortSide::EAST);
    node__D_procdff_D_43_p0->position = Point(31, 10);
    auto node__D_procdff_D_43_p1 = node__D_procdff_D_43->addPort("$procdff$43.CLK", PortSide::NORTH);
    node__D_procdff_D_43_p1->position = Point(-1, 30);
    auto node__D_procdff_D_43_p2 = node__D_procdff_D_43->addPort("$procdff$43.C", PortSide::NORTH);
    node__D_procdff_D_43_p2->position = Point(-1, 30);
    auto node__D_procdff_D_43_p3 = node__D_procdff_D_43->addPort("$procdff$43.D", PortSide::NORTH);
    node__D_procdff_D_43_p3->position = Point(-1, 10);
    auto node__D_procdff_D_43_p4 = node__D_procdff_D_43->addPort("$procdff$43.ARST", PortSide::NORTH);
    node__D_procdff_D_43_p4->position = Point(15, 40);
    auto node__D_procdff_D_43_p5 = node__D_procdff_D_43->addPort("$procdff$43.SRST", PortSide::NORTH);
    node__D_procdff_D_43_p5->position = Point(15, 40);

    auto node__D_procmux_D_36 = root->addChild("$procmux$36");
    node__D_procmux_D_36->size = Size(24, 40);
    nodeMap["$procmux$36"] = node__D_procmux_D_36;
    auto node__D_procmux_D_36_p0 = node__D_procmux_D_36->addPort("$procmux$36.A", PortSide::NORTH);
    node__D_procmux_D_36_p0->position = Point(-1, 10);
    auto node__D_procmux_D_36_p1 = node__D_procmux_D_36->addPort("$procmux$36.B", PortSide::NORTH);
    node__D_procmux_D_36_p1->position = Point(-1, 30);
    auto node__D_procmux_D_36_p2 = node__D_procmux_D_36->addPort("$procmux$36.S", PortSide::NORTH);
    node__D_procmux_D_36_p2->position = Point(12, 38);
    auto node__D_procmux_D_36_p3 = node__D_procmux_D_36->addPort("$procmux$36.Y", PortSide::EAST);
    node__D_procmux_D_36_p3->position = Point(24.5, 20);

    auto node__D_procmux_D_37_CMP0 = root->addChild("$procmux$37_CMP0");
    node__D_procmux_D_37_CMP0->size = Size(25, 25);
    nodeMap["$procmux$37_CMP0"] = node__D_procmux_D_37_CMP0;
    auto node__D_procmux_D_37_CMP0_p0 = node__D_procmux_D_37_CMP0->addPort("$procmux$37_CMP0.A", PortSide::NORTH);
    node__D_procmux_D_37_CMP0_p0->position = Point(2, 5);
    auto node__D_procmux_D_37_CMP0_p1 = node__D_procmux_D_37_CMP0->addPort("$procmux$37_CMP0.B", PortSide::NORTH);
    node__D_procmux_D_37_CMP0_p1->position = Point(2, 20);
    auto node__D_procmux_D_37_CMP0_p2 = node__D_procmux_D_37_CMP0->addPort("$procmux$37_CMP0.Y", PortSide::EAST);
    node__D_procmux_D_37_CMP0_p2->position = Point(25, 12.5);

    auto node__D_procmux_D_38_CMP0 = root->addChild("$procmux$38_CMP0");
    node__D_procmux_D_38_CMP0->size = Size(25, 25);
    nodeMap["$procmux$38_CMP0"] = node__D_procmux_D_38_CMP0;
    auto node__D_procmux_D_38_CMP0_p0 = node__D_procmux_D_38_CMP0->addPort("$procmux$38_CMP0.A", PortSide::NORTH);
    node__D_procmux_D_38_CMP0_p0->position = Point(2, 5);
    auto node__D_procmux_D_38_CMP0_p1 = node__D_procmux_D_38_CMP0->addPort("$procmux$38_CMP0.B", PortSide::NORTH);
    node__D_procmux_D_38_CMP0_p1->position = Point(2, 20);
    auto node__D_procmux_D_38_CMP0_p2 = node__D_procmux_D_38_CMP0->addPort("$procmux$38_CMP0.Y", PortSide::EAST);
    node__D_procmux_D_38_CMP0_p2->position = Point(25, 12.5);

    auto node__D_procmux_D_39_CMP0 = root->addChild("$procmux$39_CMP0");
    node__D_procmux_D_39_CMP0->size = Size(25, 25);
    nodeMap["$procmux$39_CMP0"] = node__D_procmux_D_39_CMP0;
    auto node__D_procmux_D_39_CMP0_p0 = node__D_procmux_D_39_CMP0->addPort("$procmux$39_CMP0.A", PortSide::NORTH);
    node__D_procmux_D_39_CMP0_p0->position = Point(2, 5);
    auto node__D_procmux_D_39_CMP0_p1 = node__D_procmux_D_39_CMP0->addPort("$procmux$39_CMP0.B", PortSide::NORTH);
    node__D_procmux_D_39_CMP0_p1->position = Point(2, 20);
    auto node__D_procmux_D_39_CMP0_p2 = node__D_procmux_D_39_CMP0->addPort("$procmux$39_CMP0.Y", PortSide::EAST);
    node__D_procmux_D_39_CMP0_p2->position = Point(25, 12.5);

    auto node__D_reduce_xor_D_input_P_v_C_27_D_4 = root->addChild("$reduce_xor$input.v:27$4");
    node__D_reduce_xor_D_input_P_v_C_27_D_4->size = Size(33, 25);
    nodeMap["$reduce_xor$input.v:27$4"] = node__D_reduce_xor_D_input_P_v_C_27_D_4;
    auto node__D_reduce_xor_D_input_P_v_C_27_D_4_p0 = node__D_reduce_xor_D_input_P_v_C_27_D_4->addPort("$reduce_xor$input.v:27$4.A", PortSide::NORTH);
    node__D_reduce_xor_D_input_P_v_C_27_D_4_p0->position = Point(2, 5);
    auto node__D_reduce_xor_D_input_P_v_C_27_D_4_p1 = node__D_reduce_xor_D_input_P_v_C_27_D_4->addPort("$reduce_xor$input.v:27$4.B", PortSide::NORTH);
    node__D_reduce_xor_D_input_P_v_C_27_D_4_p1->position = Point(2, 20);
    auto node__D_reduce_xor_D_input_P_v_C_27_D_4_p2 = node__D_reduce_xor_D_input_P_v_C_27_D_4->addPort("$reduce_xor$input.v:27$4.Y", PortSide::EAST);
    node__D_reduce_xor_D_input_P_v_C_27_D_4_p2->position = Point(33, 12.5);

    auto node__D_sub_D_input_P_v_C_16_D_2 = root->addChild("$sub$input.v:16$2");
    node__D_sub_D_input_P_v_C_16_D_2->size = Size(25, 25);
    nodeMap["$sub$input.v:16$2"] = node__D_sub_D_input_P_v_C_16_D_2;
    auto node__D_sub_D_input_P_v_C_16_D_2_p0 = node__D_sub_D_input_P_v_C_16_D_2->addPort("$sub$input.v:16$2.A", PortSide::NORTH);
    node__D_sub_D_input_P_v_C_16_D_2_p0->position = Point(2, 5);
    auto node__D_sub_D_input_P_v_C_16_D_2_p1 = node__D_sub_D_input_P_v_C_16_D_2->addPort("$sub$input.v:16$2.B", PortSide::NORTH);
    node__D_sub_D_input_P_v_C_16_D_2_p1->position = Point(2, 20);
    auto node__D_sub_D_input_P_v_C_16_D_2_p2 = node__D_sub_D_input_P_v_C_16_D_2->addPort("$sub$input.v:16$2.Y", PortSide::EAST);
    node__D_sub_D_input_P_v_C_16_D_2_p2->position = Point(25, 12.5);

    auto node_clock = root->addChild("clock");
    node_clock->size = Size(30, 20);
    nodeMap["clock"] = node_clock;
    auto node_clock_p0 = node_clock->addPort("clock.Y", PortSide::EAST);
    node_clock_p0->position = Point(30, 10);

    auto node_data_in = root->addChild("data_in");
    node_data_in->size = Size(30, 20);
    nodeMap["data_in"] = node_data_in;
    auto node_data_in_p0 = node_data_in->addPort("data_in.Y", PortSide::EAST);
    node_data_in_p0->position = Point(30, 10);

    auto node_up = root->addChild("up");
    node_up->size = Size(30, 20);
    nodeMap["up"] = node_up;
    auto node_up_p0 = node_up->addPort("up.Y", PortSide::EAST);
    node_up_p0->position = Point(30, 10);

    auto node_down = root->addChild("down");
    node_down->size = Size(30, 20);
    nodeMap["down"] = node_down;
    auto node_down_p0 = node_down->addPort("down.Y", PortSide::EAST);
    node_down_p0->position = Point(30, 10);

    auto node_carry_out = root->addChild("carry_out");
    node_carry_out->size = Size(30, 20);
    nodeMap["carry_out"] = node_carry_out;
    auto node_carry_out_p0 = node_carry_out->addPort("carry_out.A", PortSide::WEST);
    node_carry_out_p0->position = Point(0, 10);

    auto node_borrow_out = root->addChild("borrow_out");
    node_borrow_out->size = Size(30, 20);
    nodeMap["borrow_out"] = node_borrow_out;
    auto node_borrow_out_p0 = node_borrow_out->addPort("borrow_out.A", PortSide::WEST);
    node_borrow_out_p0->position = Point(0, 10);

    auto node_count_out = root->addChild("count_out");
    node_count_out->size = Size(30, 20);
    nodeMap["count_out"] = node_count_out;
    auto node_count_out_p0 = node_count_out->addPort("count_out.A", PortSide::WEST);
    node_count_out_p0->position = Point(0, 10);

    auto node_parity_out = root->addChild("parity_out");
    node_parity_out->size = Size(30, 20);
    nodeMap["parity_out"] = node_parity_out;
    auto node_parity_out_p0 = node_parity_out->addPort("parity_out.A", PortSide::WEST);
    node_parity_out_p0->position = Point(0, 10);

    auto node_11 = root->addChild("11");
    node_11->size = Size(30, 20);
    nodeMap["11"] = node_11;
    auto node_11_p0 = node_11->addPort("11.Y", PortSide::EAST);
    node_11_p0->position = Point(31, 10);

    auto node_10 = root->addChild("10");
    node_10->size = Size(30, 20);
    nodeMap["10"] = node_10;
    auto node_10_p0 = node_10->addPort("10.Y", PortSide::EAST);
    node_10_p0->position = Point(31, 10);

    auto node_01 = root->addChild("01");
    node_01->size = Size(30, 20);
    nodeMap["01"] = node_01;
    auto node_01_p0 = node_01->addPort("01.Y", PortSide::EAST);
    node_01_p0->position = Point(31, 10);

    auto node_00 = root->addChild("00");
    node_00->size = Size(30, 20);
    nodeMap["00"] = node_00;
    auto node_00_p0 = node_00->addPort("00.Y", PortSide::EAST);
    node_00_p0->position = Point(31, 10);

    auto node_101 = root->addChild("101");
    node_101->size = Size(30, 20);
    nodeMap["101"] = node_101;
    auto node_101_p0 = node_101->addPort("101.Y", PortSide::EAST);
    node_101_p0->position = Point(31, 10);

    auto node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11_ = root->addChild("$join$,26,27,28,29,30,31,32,33,34,49,50,51,52,53,54,55,56,57,3,4,5,6,7,8,9,10,11,");
    node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11_->size = Size(4, 60);
    nodeMap["$join$,26,27,28,29,30,31,32,33,34,49,50,51,52,53,54,55,56,57,3,4,5,6,7,8,9,10,11,"] = node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11_;
    auto node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p0 = node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11_->addPort("$join$,26,27,28,29,30,31,32,33,34,49,50,51,52,53,54,55,56,57,3,4,5,6,7,8,9,10,11,.0:8", PortSide::WEST);
    node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p0->position = Point(0, 10);
    auto node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p1 = node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11_->addPort("$join$,26,27,28,29,30,31,32,33,34,49,50,51,52,53,54,55,56,57,3,4,5,6,7,8,9,10,11,.9:17", PortSide::WEST);
    node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p1->position = Point(0, 30);
    auto node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p2 = node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11_->addPort("$join$,26,27,28,29,30,31,32,33,34,49,50,51,52,53,54,55,56,57,3,4,5,6,7,8,9,10,11,.18:26", PortSide::WEST);
    node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p2->position = Point(0, 50);
    auto node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p3 = node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11_->addPort("$join$,26,27,28,29,30,31,32,33,34,49,50,51,52,53,54,55,56,57,3,4,5,6,7,8,9,10,11,.Y", PortSide::EAST);
    node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p3->position = Point(5, 20);

    auto node__D_join_D__58_59_60_ = root->addChild("$join$,58,59,60,");
    node__D_join_D__58_59_60_->size = Size(4, 60);
    nodeMap["$join$,58,59,60,"] = node__D_join_D__58_59_60_;
    auto node__D_join_D__58_59_60__p0 = node__D_join_D__58_59_60_->addPort("$join$,58,59,60,.0", PortSide::WEST);
    node__D_join_D__58_59_60__p0->position = Point(0, 10);
    auto node__D_join_D__58_59_60__p1 = node__D_join_D__58_59_60_->addPort("$join$,58,59,60,.1", PortSide::WEST);
    node__D_join_D__58_59_60__p1->position = Point(0, 30);
    auto node__D_join_D__58_59_60__p2 = node__D_join_D__58_59_60_->addPort("$join$,58,59,60,.2", PortSide::WEST);
    node__D_join_D__58_59_60__p2->position = Point(0, 50);
    auto node__D_join_D__58_59_60__p3 = node__D_join_D__58_59_60_->addPort("$join$,58,59,60,.Y", PortSide::EAST);
    node__D_join_D__58_59_60__p3->position = Point(5, 20);

    auto node__D_join_D__13_12_ = root->addChild("$join$,13,12,");
    node__D_join_D__13_12_->size = Size(4, 40);
    nodeMap["$join$,13,12,"] = node__D_join_D__13_12_;
    auto node__D_join_D__13_12__p0 = node__D_join_D__13_12_->addPort("$join$,13,12,.0", PortSide::WEST);
    node__D_join_D__13_12__p0->position = Point(0, 10);
    auto node__D_join_D__13_12__p1 = node__D_join_D__13_12_->addPort("$join$,13,12,.1", PortSide::WEST);
    node__D_join_D__13_12__p1->position = Point(0, 30);
    auto node__D_join_D__13_12__p2 = node__D_join_D__13_12_->addPort("$join$,13,12,.Y", PortSide::EAST);
    node__D_join_D__13_12__p2->position = Point(5, 20);

    auto node__D_split_D__26_27_28_29_30_31_32_33_34_35_ = root->addChild("$split$,26,27,28,29,30,31,32,33,34,35,");
    node__D_split_D__26_27_28_29_30_31_32_33_34_35_->size = Size(5, 40);
    nodeMap["$split$,26,27,28,29,30,31,32,33,34,35,"] = node__D_split_D__26_27_28_29_30_31_32_33_34_35_;
    auto node__D_split_D__26_27_28_29_30_31_32_33_34_35__p0 = node__D_split_D__26_27_28_29_30_31_32_33_34_35_->addPort("$split$,26,27,28,29,30,31,32,33,34,35,.A", PortSide::WEST);
    node__D_split_D__26_27_28_29_30_31_32_33_34_35__p0->position = Point(0, 20);
    auto node__D_split_D__26_27_28_29_30_31_32_33_34_35__p1 = node__D_split_D__26_27_28_29_30_31_32_33_34_35_->addPort("$split$,26,27,28,29,30,31,32,33,34,35,.9", PortSide::EAST);
    node__D_split_D__26_27_28_29_30_31_32_33_34_35__p1->position = Point(4, 10);
    auto node__D_split_D__26_27_28_29_30_31_32_33_34_35__p2 = node__D_split_D__26_27_28_29_30_31_32_33_34_35_->addPort("$split$,26,27,28,29,30,31,32,33,34,35,.0:8", PortSide::EAST);
    node__D_split_D__26_27_28_29_30_31_32_33_34_35__p2->position = Point(4, 30);

    auto node__D_split_D__49_50_51_52_53_54_55_56_57_37_ = root->addChild("$split$,49,50,51,52,53,54,55,56,57,37,");
    node__D_split_D__49_50_51_52_53_54_55_56_57_37_->size = Size(5, 40);
    nodeMap["$split$,49,50,51,52,53,54,55,56,57,37,"] = node__D_split_D__49_50_51_52_53_54_55_56_57_37_;
    auto node__D_split_D__49_50_51_52_53_54_55_56_57_37__p0 = node__D_split_D__49_50_51_52_53_54_55_56_57_37_->addPort("$split$,49,50,51,52,53,54,55,56,57,37,.A", PortSide::WEST);
    node__D_split_D__49_50_51_52_53_54_55_56_57_37__p0->position = Point(0, 20);
    auto node__D_split_D__49_50_51_52_53_54_55_56_57_37__p1 = node__D_split_D__49_50_51_52_53_54_55_56_57_37_->addPort("$split$,49,50,51,52,53,54,55,56,57,37,.9", PortSide::EAST);
    node__D_split_D__49_50_51_52_53_54_55_56_57_37__p1->position = Point(4, 10);
    auto node__D_split_D__49_50_51_52_53_54_55_56_57_37__p2 = node__D_split_D__49_50_51_52_53_54_55_56_57_37_->addPort("$split$,49,50,51,52,53,54,55,56,57,37,.0:8", PortSide::EAST);
    node__D_split_D__49_50_51_52_53_54_55_56_57_37__p2->position = Point(4, 30);

    // Create edges
    auto e0 = root->addEdge("e0");
    e0->sourcePorts.push_back(node__D_procdff_D_40_p0);
    node__D_procdff_D_40_p0->outgoingEdges.push_back(e0);

    auto e1 = root->addEdge("e1");
    e1->sourcePorts.push_back(node__D_procdff_D_40_p0);
    node__D_procdff_D_40_p0->outgoingEdges.push_back(e1);
    e1->targetPorts.push_back(node__D_procmux_D_36_p0);
    node__D_procmux_D_36_p0->incomingEdges.push_back(e1);

    auto e2 = root->addEdge("e2");
    e2->sourcePorts.push_back(node__D_procdff_D_40_p0);
    node__D_procdff_D_40_p0->outgoingEdges.push_back(e2);

    auto e3 = root->addEdge("e3");
    e3->sourcePorts.push_back(node__D_procdff_D_40_p0);
    node__D_procdff_D_40_p0->outgoingEdges.push_back(e3);
    e3->targetPorts.push_back(node_count_out_p0);
    node_count_out_p0->incomingEdges.push_back(e3);

    auto e4 = root->addEdge("e4");
    e4->sourcePorts.push_back(node_11_p0);
    node_11_p0->outgoingEdges.push_back(e4);

    auto e5 = root->addEdge("e5");
    e5->sourcePorts.push_back(node_up_p0);
    node_up_p0->outgoingEdges.push_back(e5);

    auto e6 = root->addEdge("e6");
    e6->sourcePorts.push_back(node_up_p0);
    node_up_p0->outgoingEdges.push_back(e6);
    e6->targetPorts.push_back(node__D_join_D__13_12__p1);
    node__D_join_D__13_12__p1->incomingEdges.push_back(e6);

    auto e7 = root->addEdge("e7");
    e7->sourcePorts.push_back(node__D_split_D__26_27_28_29_30_31_32_33_34_35__p1);
    node__D_split_D__26_27_28_29_30_31_32_33_34_35__p1->outgoingEdges.push_back(e7);

    auto e8 = root->addEdge("e8");
    e8->sourcePorts.push_back(node_down_p0);
    node_down_p0->outgoingEdges.push_back(e8);

    auto e9 = root->addEdge("e9");
    e9->sourcePorts.push_back(node_down_p0);
    node_down_p0->outgoingEdges.push_back(e9);
    e9->targetPorts.push_back(node__D_join_D__13_12__p0);
    node__D_join_D__13_12__p0->incomingEdges.push_back(e9);

    auto e10 = root->addEdge("e10");
    e10->sourcePorts.push_back(node__D_split_D__49_50_51_52_53_54_55_56_57_37__p1);
    node__D_split_D__49_50_51_52_53_54_55_56_57_37__p1->outgoingEdges.push_back(e10);

    auto e11 = root->addEdge("e11");
    e11->sourcePorts.push_back(node_clock_p0);
    node_clock_p0->outgoingEdges.push_back(e11);
    e11->targetPorts.push_back(node__D_procdff_D_40_p1);
    node__D_procdff_D_40_p1->incomingEdges.push_back(e11);

    auto e12 = root->addEdge("e12");
    e12->sourcePorts.push_back(node_clock_p0);
    node_clock_p0->outgoingEdges.push_back(e12);
    e12->targetPorts.push_back(node__D_procdff_D_41_p1);
    node__D_procdff_D_41_p1->incomingEdges.push_back(e12);

    auto e13 = root->addEdge("e13");
    e13->sourcePorts.push_back(node_clock_p0);
    node_clock_p0->outgoingEdges.push_back(e13);
    e13->targetPorts.push_back(node__D_procdff_D_42_p1);
    node__D_procdff_D_42_p1->incomingEdges.push_back(e13);

    auto e14 = root->addEdge("e14");
    e14->sourcePorts.push_back(node_clock_p0);
    node_clock_p0->outgoingEdges.push_back(e14);
    e14->targetPorts.push_back(node__D_procdff_D_43_p1);
    node__D_procdff_D_43_p1->incomingEdges.push_back(e14);

    auto e15 = root->addEdge("e15");
    e15->sourcePorts.push_back(node__D_procmux_D_36_p3);
    node__D_procmux_D_36_p3->outgoingEdges.push_back(e15);
    e15->targetPorts.push_back(node__D_procdff_D_40_p3);
    node__D_procdff_D_40_p3->incomingEdges.push_back(e15);

    auto e16 = root->addEdge("e16");
    e16->sourcePorts.push_back(node__D_procmux_D_36_p3);
    node__D_procmux_D_36_p3->outgoingEdges.push_back(e16);

    auto e17 = root->addEdge("e17");
    e17->targetPorts.push_back(node__D_procdff_D_41_p3);
    node__D_procdff_D_41_p3->incomingEdges.push_back(e17);

    auto e18 = root->addEdge("e18");
    e18->targetPorts.push_back(node__D_procdff_D_42_p3);
    node__D_procdff_D_42_p3->incomingEdges.push_back(e18);

    auto e19 = root->addEdge("e19");
    e19->targetPorts.push_back(node__D_procdff_D_43_p3);
    node__D_procdff_D_43_p3->incomingEdges.push_back(e19);

    auto e20 = root->addEdge("e20");
    e20->sourcePorts.push_back(node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p3);
    node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p3->outgoingEdges.push_back(e20);
    e20->targetPorts.push_back(node__D_procmux_D_36_p1);
    node__D_procmux_D_36_p1->incomingEdges.push_back(e20);

    auto e21 = root->addEdge("e21");
    e21->sourcePorts.push_back(node__D_join_D__58_59_60__p3);
    node__D_join_D__58_59_60__p3->outgoingEdges.push_back(e21);
    e21->targetPorts.push_back(node__D_procmux_D_36_p2);
    node__D_procmux_D_36_p2->incomingEdges.push_back(e21);

    auto e22 = root->addEdge("e22");
    e22->sourcePorts.push_back(node__D_join_D__13_12__p2);
    node__D_join_D__13_12__p2->outgoingEdges.push_back(e22);
    e22->targetPorts.push_back(node__D_procmux_D_37_CMP0_p0);
    node__D_procmux_D_37_CMP0_p0->incomingEdges.push_back(e22);

    auto e23 = root->addEdge("e23");
    e23->sourcePorts.push_back(node__D_join_D__13_12__p2);
    node__D_join_D__13_12__p2->outgoingEdges.push_back(e23);
    e23->targetPorts.push_back(node__D_procmux_D_38_CMP0_p0);
    node__D_procmux_D_38_CMP0_p0->incomingEdges.push_back(e23);

    auto e24 = root->addEdge("e24");
    e24->sourcePorts.push_back(node__D_join_D__13_12__p2);
    node__D_join_D__13_12__p2->outgoingEdges.push_back(e24);
    e24->targetPorts.push_back(node__D_procmux_D_39_CMP0_p0);
    node__D_procmux_D_39_CMP0_p0->incomingEdges.push_back(e24);

    auto e25 = root->addEdge("e25");
    e25->sourcePorts.push_back(node_10_p0);
    node_10_p0->outgoingEdges.push_back(e25);
    e25->targetPorts.push_back(node__D_procmux_D_37_CMP0_p1);
    node__D_procmux_D_37_CMP0_p1->incomingEdges.push_back(e25);

    auto e26 = root->addEdge("e26");
    e26->sourcePorts.push_back(node_01_p0);
    node_01_p0->outgoingEdges.push_back(e26);
    e26->targetPorts.push_back(node__D_procmux_D_38_CMP0_p1);
    node__D_procmux_D_38_CMP0_p1->incomingEdges.push_back(e26);

    auto e27 = root->addEdge("e27");
    e27->sourcePorts.push_back(node_00_p0);
    node_00_p0->outgoingEdges.push_back(e27);
    e27->targetPorts.push_back(node__D_procmux_D_39_CMP0_p1);
    node__D_procmux_D_39_CMP0_p1->incomingEdges.push_back(e27);

    auto e28 = root->addEdge("e28");
    e28->sourcePorts.push_back(node_101_p0);
    node_101_p0->outgoingEdges.push_back(e28);

    auto e29 = root->addEdge("e29");
    e29->sourcePorts.push_back(node__D_procdff_D_41_p0);
    node__D_procdff_D_41_p0->outgoingEdges.push_back(e29);
    e29->targetPorts.push_back(node_carry_out_p0);
    node_carry_out_p0->incomingEdges.push_back(e29);

    auto e30 = root->addEdge("e30");
    e30->sourcePorts.push_back(node__D_procdff_D_42_p0);
    node__D_procdff_D_42_p0->outgoingEdges.push_back(e30);
    e30->targetPorts.push_back(node_borrow_out_p0);
    node_borrow_out_p0->incomingEdges.push_back(e30);

    auto e31 = root->addEdge("e31");
    e31->sourcePorts.push_back(node__D_procdff_D_43_p0);
    node__D_procdff_D_43_p0->outgoingEdges.push_back(e31);
    e31->targetPorts.push_back(node_parity_out_p0);
    node_parity_out_p0->incomingEdges.push_back(e31);

    auto e32 = root->addEdge("e32");
    e32->sourcePorts.push_back(node__D_split_D__26_27_28_29_30_31_32_33_34_35__p2);
    node__D_split_D__26_27_28_29_30_31_32_33_34_35__p2->outgoingEdges.push_back(e32);
    e32->targetPorts.push_back(node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p0);
    node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p0->incomingEdges.push_back(e32);

    auto e33 = root->addEdge("e33");
    e33->sourcePorts.push_back(node__D_split_D__49_50_51_52_53_54_55_56_57_37__p2);
    node__D_split_D__49_50_51_52_53_54_55_56_57_37__p2->outgoingEdges.push_back(e33);
    e33->targetPorts.push_back(node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p1);
    node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p1->incomingEdges.push_back(e33);

    auto e34 = root->addEdge("e34");
    e34->sourcePorts.push_back(node_data_in_p0);
    node_data_in_p0->outgoingEdges.push_back(e34);
    e34->targetPorts.push_back(node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p2);
    node__D_join_D__26_27_28_29_30_31_32_33_34_49_50_51_52_53_54_55_56_57_3_4_5_6_7_8_9_10_11__p2->incomingEdges.push_back(e34);

    auto e35 = root->addEdge("e35");
    e35->sourcePorts.push_back(node__D_procmux_D_37_CMP0_p2);
    node__D_procmux_D_37_CMP0_p2->outgoingEdges.push_back(e35);
    e35->targetPorts.push_back(node__D_join_D__58_59_60__p0);
    node__D_join_D__58_59_60__p0->incomingEdges.push_back(e35);

    auto e36 = root->addEdge("e36");
    e36->sourcePorts.push_back(node__D_procmux_D_38_CMP0_p2);
    node__D_procmux_D_38_CMP0_p2->outgoingEdges.push_back(e36);
    e36->targetPorts.push_back(node__D_join_D__58_59_60__p1);
    node__D_join_D__58_59_60__p1->incomingEdges.push_back(e36);

    auto e37 = root->addEdge("e37");
    e37->sourcePorts.push_back(node__D_procmux_D_39_CMP0_p2);
    node__D_procmux_D_39_CMP0_p2->outgoingEdges.push_back(e37);
    e37->targetPorts.push_back(node__D_join_D__58_59_60__p2);
    node__D_join_D__58_59_60__p2->incomingEdges.push_back(e37);

    auto e38 = root->addEdge("e38");
    e38->targetPorts.push_back(node__D_split_D__26_27_28_29_30_31_32_33_34_35__p0);
    node__D_split_D__26_27_28_29_30_31_32_33_34_35__p0->incomingEdges.push_back(e38);

    auto e39 = root->addEdge("e39");
    e39->targetPorts.push_back(node__D_split_D__49_50_51_52_53_54_55_56_57_37__p0);
    node__D_split_D__49_50_51_52_53_54_55_56_57_37__p0->incomingEdges.push_back(e39);

    // Run layout
    layered::LayeredLayoutProvider layout;
    layout.setDirection(Direction::RIGHT);
    layout.setNodeSpacing(35.0);
    layout.setLayerSpacing(35.0);
    layout.layout(root.get(), nullptr);

    // Print results
    std::cout << "=== C++ ELK Layout Results ===" << std::endl;
    std::cout << "Graph size: " << root->size.width << " x " << root->size.height << std::endl;
    std::cout << "\nNode Positions:" << std::endl;
    for (const auto& child : root->children) {
        std::cout << "  " << child->id << ": (" << child->position.x << ", " << child->position.y << ")" << std::endl;
    }
    return 0;
}
