import os

def enable_stp_for_switches():
    core_switch_num = 4
    pod_num = 4
    pod_switch_ae_num = 2

    for i in range(core_switch_num):
        switch_name = f"core{i}"
        os.system(f"sudo ovs-vsctl set Bridge {switch_name} stp_enable=true")
        os.system(f"sudo ovs-vsctl del-fail-mode {switch_name}")

    for i in range(pod_num):
        for j in range(pod_switch_ae_num):
            aggr_switch_name = f"Arpod{i}no{j}"
            edge_switch_name = f"Edpod{i}no{j}"

            os.system(f"sudo ovs-vsctl set Bridge {aggr_switch_name} stp_enable=true")
            os.system(f"sudo ovs-vsctl del-fail-mode {aggr_switch_name}")

            os.system(f"sudo ovs-vsctl set Bridge {edge_switch_name} stp_enable=true")
            os.system(f"sudo ovs-vsctl del-fail-mode {edge_switch_name}")

if __name__ == '__main__':
    enable_stp_for_switches()
