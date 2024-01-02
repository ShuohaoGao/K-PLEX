# 导入 numpy 和 matplotlib.pyplot 库
import numpy as np
import matplotlib.pyplot as plt

legend_font_size = 16
x_label_size = 20
x_ticks_size = 16
y_label_size = 20
y_ticks_size = 16
bar_width = 0.21
fig_size_inches = (5.5, 4)
fig_size = (4, 3)
ax_position = [0.14, 0.17, 0.85, 0.68]


def draw_lines_solved_instance_cnt(x_ticks, y, legend, file_name='./img/test_line.pdf',
                                   y_name='#solved instances', x_name='time(sec)', y_range=None, y_tick=None):
    """
    :param x_ticks:x轴标签
    :param y: 共len(y)条折线,每条折线需要提供len(x_ticks)个数值
    :param legend: 每条折线的注释，len(legend) = len(y)
    :param file_name: 保存的路径
    :param y_name: y轴的名称
    :param x_name: x轴的名称
    :param y_range: y轴范围
    :param y_tick: y轴刻度的标签
    """
    plt.close()
    plt.figure(figsize=fig_size)
    # 定义变量 markers，表示不同线条的样式
    markers = ['r-^', 'g-o', 'b-X', 'k-s']

    # 定义变量 x，表示 x 轴标签的位置
    x = np.arange(len(x_ticks))

    # 定义变量 group，表示算法的数量
    group = len(y)

    y = np.array(y)
    # 循环遍历每个算法，并绘制折线图
    for i in range(0, group):
        plt.plot(x,
                 y[i],
                 markers[i],
                 markerfacecolor='none',
                 markersize='8',
                 label=legend[i],
                 clip_on=False,
                 linewidth=2)

    # 设置 y 轴标签和刻度
    plt.ylabel(y_name, fontsize=y_label_size, labelpad=-3)
    plt.yticks(
        # [0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000, 86400],
        # ['$10^{-3}$', '$10^{-2}$', '$10^{-1}$', '$10^{0}$', '$10^1$', '$10^2$', '$10^3$', '$10^4$', '86400'],
        fontsize=y_ticks_size
    )
    if y_range is None or len(y_range) < 2:
        lim = [int(np.min(y) - 1), int(np.max(y) + 1)]
    else:
        lim = y_range
    plt.ylim(lim)
    if y_tick is None or len(y_tick) < 4:
        plt.yticks(range(lim[0], lim[1] + 1, (lim[1] - lim[0]) // 5))
    else:
        # temp = [f'$\leq{lim[0]}$'] + [str(tick) for tick in y_tick[1:]]
        # plt.yticks(y_tick, temp)
        plt.yticks(y_tick)

    # 设置 x 轴标签和刻度
    plt.xlabel(xlabel=x_name, fontsize=x_label_size, labelpad=-5.5)
    plt.xticks(x, x_ticks, fontsize=x_ticks_size, rotation=20)

    # 添加网格线，并设置其样式和宽度
    plt.grid(axis='y', linestyle='--', linewidth=0.5)
    plt.minorticks_off()

    # 添加图例，并设置其位置和字体大小
    plt.legend(fontsize=legend_font_size)

    # 调整图形布局，并设置其大小
    fig = plt.gcf()
    fig.set_size_inches(fig_size_inches)
    plt.subplots_adjust(left=ax_position[0], bottom=ax_position[1], right=ax_position[2] + ax_position[0],
                        top=ax_position[3] + ax_position[1])

    plt.legend(bbox_to_anchor=(0, 0.94, 1, 0), loc="lower left", mode="expand", ncol=2, fontsize=legend_font_size,
               frameon=False)

    # 将图形保存为 PDF 文件，并设置其 DPI 和边框范围
    plt.savefig(file_name, dpi=400)
    plt.close(fig)


def draw_lines_time_auto_scale(x_ticks, y, legend, file_name='./img/test_line.pdf',
                               y_name='time(sec)', x_name='$k,\\ell$', always_positive=False,
                               log_scale=False):
    plt.close()
    plt.figure(figsize=fig_size)
    # 定义变量 markers，表示不同线条的样式
    markers = ['k-o', 'k-^']

    # 定义变量 x，表示 x 轴标签的位置
    x = np.arange(len(x_ticks))

    # 定义变量 group，表示算法的数量
    group = len(y)

    y = np.array(y)
    max_y = np.max(y)
    min_y = np.min(y)
    y_scale = (max_y - min_y) / 3
    # 循环遍历每个算法，并绘制折线图
    for i in range(0, group):
        plt.plot(x,
                 y[i],
                 markers[i],
                 markerfacecolor='none',
                 markersize='10',
                 label=legend[i],
                 clip_on=False,
                 linewidth=1)

    # 设置 y 轴标签和刻度
    plt.ylabel(y_name, fontsize=y_label_size, labelpad=0)
    plt.yticks(fontsize=y_ticks_size)
    if always_positive:
        l = max(min_y - y_scale, 0.001)
        r = max_y + y_scale * 4
        plt.ylim(l, r)
    else:
        plt.ylim(min_y - y_scale, max_y + y_scale * 4)
    if log_scale:
        plt.yscale('log')
        # plt.yticks([0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000, 86400],
        #            ['$10^{-3}$', '$10^{-2}$', '$10^{-1}$', '$10^{0}$', '$10^1$', '$10^2$', '$10^3$', '$10^4$', ''],
        #            fontsize=y_ticks_size)
        plt.yticks([0.001, 0.01, 0.1, 1, 10, 100, 1000, 10000, 86400],
                   ['', '$10^{-2}$', '', '$10^{0}$', '', '$10^2$', '', '$10^4$', ''],
                   fontsize=y_ticks_size)

    # 设置 x 轴标签和刻度
    plt.xlabel(xlabel=x_name, fontsize=x_label_size, labelpad=-4)
    plt.xticks(x, x_ticks, fontsize=x_ticks_size, rotation=30)

    # 添加网格线，并设置其样式和宽度
    plt.grid(axis='y', linestyle='--', linewidth=0.5)
    plt.minorticks_off()

    # 添加图例，并设置其位置和字体大小
    plt.legend(fontsize=legend_font_size)

    # 调整图形布局，并设置其大小
    fig = plt.gcf()
    fig.set_size_inches(fig_size_inches)
    plt.subplots_adjust(left=ax_position[0], bottom=ax_position[1], right=ax_position[2] + ax_position[0],
                        top=ax_position[3] + ax_position[1])

    plt.legend(bbox_to_anchor=(0, 0.7, 1, 0), loc="lower left", mode="expand", ncol=1, fontsize=legend_font_size,
               frameon=False)

    # 将图形保存为 PDF 文件，并设置其 DPI 和边框范围
    plt.savefig(file_name, dpi=400)
    plt.close(fig)
