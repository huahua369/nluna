/*
	.class
	#id
*/
enum class css_ec :uint16_t {
	A,
	align_content,//规定弹性容器内的行之间的对齐方式，当项目不使用所有可用空间时。
	align_items,//规定弹性容器内项目的对齐方式。
	align_self,//规定弹性容器内所选项目的对齐方式。
	all,//重置所有属性（除了 unicode_bidi 和 direction）。
	animation,//所有 animation_* 属性的简写属性。
	animation_delay,//规定开始动画的延迟。
	animation_direction,//规定动画是向前播放、向后播放还是交替播放。
	animation_duration,//规定动画完成一个周期应花费的时间。
	animation_fill_mode,//规定元素在不播放动画时（在开始之前、结束之后、或同时）的样式。
	animation_iteration_count,//规定动画的播放次数。
	animation_name,//规定 @keyframes 动画的名称。
	animation_play_state,//规定动画是播放还是暂停。
	animation_timing_function,//规定动画的速度曲线。
	B,
	backface_visibility,//定义当面对用户时元素的背面是否应可见。
	background,//所有 background_* 属性的简写属性。
	background_attachment,//设置背景图像是与页面的其余部分一起滚动还是固定的。
	background_blend_mode,//规定每个背景图层（颜色/图像）的混合模式。
	background_clip,//定义背景（颜色或图像）应在元素内延伸的距离。
	background_color,//规定元素的背景色。
	background_image,//规定元素的一幅或多幅背景图像。
	background_origin,//规定背景图像的初始位置。
	background_position,//规定背景图像的位置。
	background_repeat,//设置是否以及如何重复背景图像。
	background_size,//规定背景图像的尺寸。
	border,//border_width、border_style 以及 border_color 的简写属性。
	border_bottom,//border_bottom_width、border_bottom_style 以及 border_bottom_color 的简写属性。
	border_bottom_color,//设置下边框的颜色。
	border_bottom_left_radius,//定义左下角的边框圆角。
	border_bottom_right_radius,//定义右下角的边框圆角。
	border_bottom_style,//设置下边框的样式。
	border_bottom_width,//设置下边框的宽度。
	border_collapse,//设置表格边框是折叠为单一边框还是分开的。
	border_color,//设置四条边框的颜色。
	border_image,//border_image_* 属性的简写属性。
	border_image_outset,//规定边框图像区域超出边框的量。
	border_image_repeat,//规定边框图像应重复、圆角、还是拉伸。
	border_image_slice,//规定如何裁切边框图像。
	border_image_source,//规定用作边框的图像的路径。
	border_image_width,//规定边框图像的宽度。
	border_left,//所有 border_left_* 属性的简写属性。
	border_left_color,//设置左边框的颜色。
	border_left_style,//设置左边框的样式。
	border_left_width,//设置左边框的宽度。
	border_radius,//四个 border_*_radius 属性的简写属性。
	border_right,//所有 border_right_* 属性的简写属性。
	border_right_color,//设置右边框的颜色。
	border_right_style,//设置右边框的样式。
	border_right_width,//设置右边框的宽度。
	border_spacing,//设置相邻单元格边框之间的距离。
	border_style,//设置四条边框的样式。
	border_top,//border_top_width、border_top_style 以及 border_top_color 的简写属性。
	border_top_color,//设置上边框的颜色。
	border_top_left_radius,//定义左上角的边框圆角。
	border_top_right_radius,//定义右上角的边框圆角。
	border_top_style,//设置上边框的样式。
	border_top_width,//设置上边框的宽度。
	border_width,//设置四条边框的宽度。
	bottom,//设置元素相对于其父元素底部的位置。
	box_decoration_break,//设置元素在分页符处的背景和边框的行为，或对于行内元素在换行符处的行为。
	box_shadow,//将一个或多个阴影附加到元素。
	box_sizing,//定义元素的宽度和高度的计算方式：它们是否应包含内边距和边框。
	break_after,//规定指定元素之后是否应出现 page_、column_ 或 region_break。
	break_before,//规定指定元素之前是否应出现 page_、column_ 或 region_break。
	break_inside,//规定指定元素内部是否应出现 page_、column_ 或 region_break。
	C,
	caption_side,//规定表格标题的放置方式。
	caret_color,//规定光标在 input、textarea 或任何可编辑元素中的颜色。
	_charset,//@规定样式表中使用的字符编码。
	clear,//规定不允许在元素的哪一侧浮动元素
	clip,//剪裁绝对定位的元素。
	clip_path,//将元素裁剪为基本形状或 SVG 源。
	color,//设置文本的颜色。
	column_count,//规定元素应分为的列数。
	column_fill,//指定如何填充列（是否 balanced）。
	column_gap,//规定列间隙。
	column_rule,//所有 column_rule_* 属性的简写属性。
	column_rule_color,//规定列之间规则的颜色。
	column_rule_style,//规定列之间的规则样式。
	column_rule_width,//规定列之间的规则宽度。
	column_span,//规定元素应该跨越多少列。
	column_width,//规定列宽度。
	columns,//column_width 和 column_count 的简写属性。
	content,//与 :before 和 :after 伪元素一起使用，来插入生成的内容。
	counter_increment,//增加或减少一个或多个 CSS 计数器的值。
	counter_reset,//创建或重置一个或多个 CSS 计数器。
	cursor,//规定当指向元素时要显示的鼠标光标。
	D,
	direction,//规定文本方向/书写方向。
	display,//规定如何显示某个 HTML 元素。
	E,
	empty_cells,//规定是否在表格中的空白单元格上显示边框和背景。
	F,
	filter,//定义元素显示之前的效果（例如，模糊或颜色偏移）。
	flex,//flex_grow、flex_shrink 以及 flex_basis 的简写属性。
	flex_basis,//规定弹性项目的初始长度。
	flex_direction,//规定弹性项目的方向。
	flex_flow,//flex_direction 和 flex_wrap 的简写属性。
	flex_grow,//规定项目相对于其余项目的增量。
	flex_shrink,//规定项目相对于其余项目的减量。
	flex_wrap,//规定弹性项目是否应该换行。
	floating,//规定是否应该对盒（box）进行浮动。
	font,//font_style、font_variant、font_weight、font_size/line_height 以及 font_family 的简写属性。
	_font_face,//允许网站下载和使用 "web_safe" 字体以外的其他字体的规则。
	font_family,//规定文本的字体族（字体系列）。
	font_feature_settings,//允许控制 OpenType 字体中的高级印刷特性。
	_font_feature_values,//允许创作者使用 font_variant_alternate 中的通用名来实现在 OpenType 中以不同方式激活的特性。
	font_kerning,//控制字距调整信息的使用（字母间距）。
	font_language_override,//控制特定语言的字形在字体的使用。
	font_size,//规定文本的字体大小。
	font_size_adjust,//保持发生字体回退时的可读性。
	font_stretch,//从字体系列中选择一个普通的、压缩的或扩展的字体。
	font_style,//规定文本的字体样式。
	font_synthesis,//控制哪些缺失的字体（粗体或斜体）可以由浏览器合成。
	font_variant,//规定是否应该以小型大写字体显示文本。
	font_variant_alternates,//控制与 @font_feature_values 中定义的备用名称关联的备用字形的使用。
	font_variant_caps,//控制大写字母的备用字形的使用。
	font_variant_east_asian,//控制东亚文字（例如中文和日语）的备用字形的使用。
	font_variant_ligatures,//控制在适用于元素的文本内容中使用哪些连字和上下文形式。
	font_variant_numeric,//控制数字、分数和序号标记的备用字形的使用。
	font_variant_position,//控制较小字体的替代字形的使用，这些字形相对于字体基线定位为上标或下标。
	font_weight,//规定字体的粗细。
	G,
	grid,//grid_template_rows、grid_template_columns、grid_template_areas、grid_auto_rows、grid_auto_columns 以及 grid_auto_flow 属性的简写属性。
	grid_area,//即可规定网格项的名称，也可以是 grid_row_start、grid_column_start、grid_row_end 以及 grid_column_end 属性的简写属性。
	grid_auto_columns,//规定默认的列尺寸。
	grid_auto_flow,//规定如何在网格中插入自动放置的项目。
	grid_auto_rows,//规定默认的行尺寸。
	grid_column,//grid_column_start 和 grid_column_end 属性的简写属性。
	grid_column_end,//规定如何结束网格项目。
	grid_column_gap,//规定列间隙的尺寸。
	grid_column_start,//规定网格项目从何处开始。
	grid_gap,//grid_row_gap 和 grid_column_gap 的简写属性。
	grid_row,//grid_row_start 和 grid_row_end 属性的简写属性。
	grid_row_end,//规定网格项目在何处结束。
	grid_row_gap,//规定列间隙的尺寸。
	grid_row_start,//规定网格项目从何处开始。
	grid_template,//grid_template_rows、grid_template_columns 以及 grid_areas 属性的简写属性。
	grid_template_areas,//规定如何使用命名的网格项显示列和行。
	grid_template_columns,//指定列的尺寸以及网格布局中的列数。
	grid_template_rows,//指定网格布局中的行的尺寸。
	H,
	hanging_punctuation,//规定是否可以在行框外放置标点符号。
	height,//设置元素的高度。
	hyphens,//设置如何分割单词以改善段落的布局。
	I,
	image_rendering,//当图像被缩放时，向浏览器提供关于保留图像的哪些最重要的方面的信息。
	_import,//@允许您将样式表导入另一张样式表。
	isolation,//定义元素是否必须创建新的堆叠内容。
	J,
	justify_content,//规定项目在弹性容器内的对齐方式，当项目未用到所有可用空间时。
	K,
	_keyframes,//@规定动画代码。
	L,
	left,//规定定位元素的左侧位置。
	letter_spacing,//增加或减少文本中的字符间距。
	line_break,//如何如何/是否换行。
	line_height,//设置行高。
	list_style,//在一条声明中设置所有列表属性。
	list_style_image,//把图像指定为列表项标记。
	list_style_position,//规定列表项标记的位置。
	list_style_type,//规定列表项标记的类型。
	M,
	margin,//在一条声明中设置所有外边距属性。
	margin_bottom,//设置元素的下外边距。
	margin_left,//设置元素的左外边距。
	margin_right,//设置元素的右外边距。
	margin_top,//设置元素的上外边距。
	mask,//通过在特定位置遮罩或剪切图像来隐藏元素。
	mask_type,//规定将遮罩元素用作亮度或 Alpha 遮罩。
	max_height,//设置元素的最大高度。
	max_width,//设置元素的最大宽度。
	_media,//@为不同的媒体类型、设备、尺寸设置样式规则。
	min_height,//设置元素的最小高度。
	min_width,//设置元素的最小宽度。
	mix_blend_mode,//规定元素内容应如何与其直接父的背景相混合。
	O,
	object_fit,//规定替换元素的内容应如何适合其所用高度和宽度建立的框。
	object_position,//指定替换元素在其框内的对齐方式。
	opacity,//设置元素的不透明等级。
	order,//设置弹性项目相对于其余项目的顺序。
	orphans,//设置在元素内发生分页时必须保留在页面底部的最小行数。
	outline,//outline_width、outline_style 以及 outline_color 属性的简写属性。
	outline_color,//设置轮廓的颜色。
	outline_offset,//对轮廓进行偏移，并将其绘制到边框边缘之外。
	outline_style,//设置轮廓的样式。
	outline_width,//设置轮廓的宽度。
	overflow,//规定如果内容溢出元素框会发生什么情况。
	overflow_wrap,//规定浏览器是否可能为了防止溢出而在单词内折行（当字符串太长而无法适应其包含框时）。
	overflow_x,//规定是否剪裁内容的左右边缘，如果它溢出了元素的内容区域。
	overflow_y,//规定是否剪裁内容的上下边缘，如果它溢出了元素的内容区域。
	P,
	padding,//所有 padding_* 属性的简写属性。
	padding_bottom,//设置元素的下内边距。
	padding_left,//设置元素的左内边距。
	padding_right,//设置元素的右内边距。
	padding_top,//设置元素的上内边距。
	page_break_after,//设置元素之后的分页（page_break）行为。
	page_break_before,//设置元素之前的分页（page_break）行为。
	page_break_inside,//设置元素内的分页（page_break）行为。
	perspective,//为 3D 定位元素提供透视。
	perspective_origin,//定义用户观看 3D 定位元素的位置。
	pointer_events,//定义元素是否对指针事件做出反应。
	position,//规定用于元素的定位方法的类型（静态、相对、绝对或固定）。
	Q,
	quotes,//设置引号类型。
	R,
	resize,//定义用户是否以及如何调整元素的尺寸。
	right,//规定定位元素的左侧位置。
	S,
	scroll_behavior,//规定可滚动框中是否平滑地滚动，而不是直接跳跃。
	T,
	tab_size,//规定制表符的宽度。
	table_layout,//定义用于对单元格、行和列进行布局的算法。
	text_align,//规定文本的水平对齐方式。
	text_align_last,//描述当 text_align 为 "justify" 时，如何在强制换行之前对齐块或行的最后一行。
	text_combine_upright,//将多个字符组合到到单个字符的空间中。
	text_decoration,//规定文本装饰。
	text_decoration_color,//规定文本装饰（text_decoration）的颜色。
	text_decoration_line,//规定文本装饰（text_decoration）中的的行类型。
	text_decoration_style,//规定文本装饰（text_decoration）中的行样式。
	text_indent,//规定文本块（text_block）中的的首行缩进。
	text_justify,//规定当 text_align 为 "justify" 时使用的对齐方法。
	text_orientation,//定义行中的文本方向。
	text_overflow,//规定当文本溢出包含元素时应该发生的情况。
	text_shadow,//添加文本阴影。
	text_transform,//控制文本的大写。
	text_underline_position,//规定使用 text_decoration 属性设置的下划线的位置。
	top,//规定定位元素的顶端位置。
	transform,//向元素应用 2D 或 3D 转换。
	transform_origin,//允许您更改转换元素的位置。
	transform_style,//规定如何在 3D 空间中渲染嵌套的元素。
	transition,//所有 transition_* 属性的简写属性。
	transition_delay,//规定合适开始过渡效果。
	transition_duration,//规定完成过渡效果所需的秒或毫秒数。
	transition_property,//规定过渡效果对应的 CSS 属性的名称。
	transition_timing_function,//规定过渡效果的速度曲线。
	U,
	unicode_bidi,//与 direction 属性一起使用，设置或返回是否应覆写文本来支持同一文档中的多种语言。
	user_select,//规定是否能选取元素的文本。
	V,
	vertical_align,//设置元素的垂直对齐方式。
	visibility,//规定元素是否可见。
	W,
	white_space,//规定如何处理元素内的空白字符。
	widows,//设置如果元素内发生分页，必须在页面顶部保留的最小行数。
	width,//设置元素的宽度。
	word_break,//规定单词到达行末后如何换行。
	word_spacing,//增加或减少文本中的单词间距。
	word_wrap,//允许长的、不能折行的单词换到下一行。
	writing_mode,//规定文本行是水平还是垂直布局。
	Z,
	z_index,//设置定位元素的堆叠顺序。
	e_max,
	css_count = e_max - 26
};
/*
	css_ec ak = css_ec::border_collapse;
	t_map<css_ec, njson> kmm;
	kmm[ak] = 123;
*/
enum class gte :uint8_t {
	e_null = 0,
	e_rect,
	e_circle,
	e_triangle,
	e_path,
	e_text,
	e_image,
	e_max
};
