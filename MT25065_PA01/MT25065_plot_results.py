import pandas as pd
import matplotlib.pyplot as plt
import os

def generate_standard_plots():
    plt.style.use('ggplot') 
    
    file_c = "MT25065_Part_C_CSV.csv"
    file_d = "MT25065_Part_D_CSV.csv"
    
    if os.path.exists(file_c):
        print(f"Processing {file_c}...")
        df_c = pd.read_csv(file_c)
        
        df_c[['Program', 'Worker']] = df_c['Program+Function'].str.split('+', expand=True)
        worker_order = ['cpu', 'mem', 'io']
        
        def plot_bar(metric, title, ylabel, filename):
            plt.figure(figsize=(10, 6))
            pivot_df = df_c.pivot(index='Worker', columns='Program', values=metric)
            pivot_df = pivot_df.reindex(worker_order)
            pivot_df.plot(kind='bar', width=0.7, ax=plt.gca(), edgecolor='black')
            plt.title(title, fontsize=14); plt.ylabel(ylabel, fontsize=12)
            plt.xlabel("Worker Type", fontsize=12); plt.xticks(rotation=0)
            plt.legend(title="Program"); plt.grid(True, axis='y', linestyle='--', alpha=0.7)
            plt.tight_layout(); plt.savefig(filename); plt.close()


        plot_bar('CPU%', 'Graph 01: Baseline CPU Usage (Part C)', 'CPU Usage (%)', 'PartC_01_CPU_Usage.png')
        

        df_c['Mem_MB'] = df_c['Mem'] / 1024
        plot_bar('Mem_MB', 'Graph 02: Baseline Memory Usage (Part C)', 'Memory Usage (MB)', 'PartC_02_Mem_Usage.png')


        plot_bar('IO', 'Graph 03: Baseline Disk I/O (Part C)', 'Throughput (Blocks/KB)', 'PartC_03_IO_Throughput.png')


        plot_bar('Execution_Time_Seconds', 'Graph 04: Baseline Execution Time (Part C)', 'Time (seconds)', 'PartC_04_Execution_Time.png')
                 
    else:
        print(f"Warning: {file_c} not found.")

    if os.path.exists(file_d):
        print(f"Processing {file_d}...")
        df_d = pd.read_csv(file_d)
        
        def plot_scaling(worker, metric, title, ylabel, filename):
            plt.figure(figsize=(10, 6))
            subset = df_d[df_d['Worker'] == worker]
            for prog in ['A', 'B']:
                data = subset[subset['Program'] == prog].sort_values('Count')
                plt.plot(data['Count'], data[metric], marker='o', linewidth=2, label=f"Program {prog}")
            plt.title(title, fontsize=14); plt.ylabel(ylabel, fontsize=12)
            plt.xlabel("Number of Processes / Threads", fontsize=12); plt.legend()
            plt.grid(True, linestyle='--', alpha=0.7); plt.tight_layout(); plt.savefig(filename); plt.close()


        plot_scaling('cpu', 'CPU_Percent', 
                     'Graph 05: CPU Usage Scaling (CPU Worker)', 
                     'CPU Usage (%)', 'PartD_05_CPU_Scaling.png')


        plot_scaling('cpu', 'Time_Seconds', 
                     'Graph 06: Execution Time Scaling (CPU Worker)', 
                     'Execution Time (s)', 'PartD_06_CPU_Time_Scaling.png')


        df_d['Mem_MB'] = df_d['Mem'] / 1024
        plot_scaling('mem', 'Mem_MB', 
                     'Graph 07: Memory Usage Scaling (Mem Worker)', 
                     'Memory Usage (MB)', 'PartD_07_Mem_Scaling.png')


        plot_scaling('io', 'Time_Seconds', 
                     'Graph 08: Execution Time Scaling (I/O Worker)', 
                     'Execution Time (s)', 'PartD_08_IO_Time_Scaling.png')
                     
    else:
        print(f"Warning: {file_d} not found.")

    print("Successfully generated 8 Standard Graphs.")

if __name__ == "__main__":
    generate_standard_plots()
