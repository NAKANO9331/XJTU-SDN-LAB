from mininet.topo import Topo
from mininet.net import Mininet
from mininet.cli import CLI
from mininet.log import setLogLevel

k = 2
class fatTree(Topo):
    def build(self):
        core_switch_num = (k // 2) ** 2
        pod_num = k
        pod_switch_ae_num = k // 2
        pod_host_num = (k // 2)
        core_switchs = []  
        aggr_switchs = [[] for _ in range(pod_num)]  
        edge_switchs = [[] for _ in range(pod_num)]  
        hosts = [[] for _ in range(pod_num)]  
        for i in range(core_switch_num):
            core_switch = self.addSwitch(f"core{i}")
            core_switchs.append(core_switch)
        for i in range(pod_num):
            for j in range(pod_switch_ae_num):
                aggr_switch = self.addSwitch(f"Arpod{i}no{j}")
                aggr_switchs[i].append(aggr_switch)
        for i in range(pod_num):
            for j in range(pod_switch_ae_num):
                edge_switch = self.addSwitch(f"Edpod{i}no{j}")
                edge_switchs[i].append(edge_switch)
        for i in range(pod_num):
            for j in range(pod_host_num):
                host = self.addHost(f"h{i}_{j}")  
                hosts[i].append(host)
        for core_index in range(core_switch_num):
            sub_index = core_index % (k // 2)
            for pod_index in range(pod_num):
                self.addLink(core_switchs[core_index], aggr_switchs[pod_index][sub_index])
        for pod_index in range(pod_num):
            for aggr_index in range(pod_switch_ae_num):
                for edge_index in range(pod_switch_ae_num):
                    self.addLink(aggr_switchs[pod_index][aggr_index], edge_switchs[pod_index][edge_index])
        for pod_index in range(pod_num):
            for edge_index in range(pod_switch_ae_num):
                for host_index in range(pod_host_num):
                    self.addLink(edge_switchs[pod_index][edge_index], hosts[pod_index][host_index])
def run():
    topo = fatTree()
    net = Mininet(topo, controller=None, autoSetMacs=True)  # 设置 autoSetMacs 为 True，自动生成 MAC 地址
    net.start()
    CLI(net)  
    net.stop()
if __name__ == '__main__':
    setLogLevel('info')
    run()
