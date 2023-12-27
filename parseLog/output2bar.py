import numpy as np
import matplotlib.pyplot as plt

legend_font_size = 16
x_label_size = 20
x_ticks_size = 16
y_label_size = 20
y_ticks_size = 16
bar_width = 0.21
fig_size_inches = (6, 4)
fig_size = (16, 9)
ax_position = [0.14, 0.27, 0.83, 0.7]


def draw_bars(x_ticks, y, legend, file_name="img/test_bar.pdf", x_name="speed ratio",
              y_name='#instances', lim=None):
    """
    :param x_ticks:x轴标签
    :param y: 共len(y)中柱子,每种柱子需要提供len(x_ticks)个数值
    :param legend: 每种柱子的注释，len(legend) = len(y)
    :param file_name: 保存的路径
    :param y_name: y轴的名称
    :param x_name: x轴的名称
    :param lim: y轴范围
    """
    y = np.array(y)
    plt.close()
    plt.figure(figsize=fig_size)
    # 定义变量 marks 和 colors，分别表示不同的填充和颜色
    marks = ["", "///", "", ""]
    colors = ["black", "w", "gray", "white"]
    if len(y) == 1:
        marks = ["///"]
        colors = ["gray"]

    # 定义变量 width，表示条形图的宽度
    width = bar_width

    # 定义变量 x，表示 x 轴标签的位置
    x = np.arange(len(x_ticks))

    # 创建一个新的图形和一个子图
    ax = plt.subplots()[1]

    # 定义变量 group，表示算法的数量
    group = len(y)

    # 循环遍历每个算法，并绘制条形图
    for i in range(0, group):
        rects = ax.bar(x - (group - 2 * i - 1) * width / 2, y[i], width, label=legend[i], color=colors[i],
                       edgecolor="k", zorder=100)
        for bar in rects:
            bar.set_hatch(marks[i])

    # 设置 y 轴标签和刻度
    ax.set_ylabel(y_name, fontsize=y_label_size, labelpad=2)
    plt.yticks(fontsize=y_ticks_size)
    if lim is None:
        ax.set_ylim((0, np.max(y) + 4))
    else:
        ax.set_ylim(lim)
    plt.locator_params(axis='y', integer=True)

    # 设置 x 轴标签和刻度
    ax.set_xticks(x)
    ax.set_xticklabels(x_ticks, fontsize=x_ticks_size, rotation=90)  # 斜40度，不那么挤

    # 添加图例，并设置其位置和字体大小：矩形区域上方，分为2列
    ax.legend(fontsize=legend_font_size)
    plt.legend(bbox_to_anchor=(0, 0.7, 1, 0), loc="lower left", mode="expand", ncol=1, fontsize=legend_font_size,
               frameon=False)

    # 添加虚线
    ax.grid(axis='y', linestyle='--', linewidth=0.5, zorder=0)
    ax.minorticks_off()
    ax.set_position(ax_position)

    # 调整图形布局，并设置其大小
    fig = plt.gcf()
    fig.set_size_inches(fig_size_inches)

    # 将图形保存为 pdf 文件，并设置其 DPI 和边框范围
    plt.savefig(file_name, dpi=400)
    plt.close(fig)
