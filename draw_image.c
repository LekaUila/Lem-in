/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   draw_image.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lflandri <liam.flandrinck.58@gmail.com>    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2024/03/12 17:26:25 by lflandri          #+#    #+#             */
/*   Updated: 2024/03/22 15:06:05 by lflandri         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "header.h"

static void	img_pix_put(void *img_mlx, int x, int y, int color)
{
	char	*pixel;
	int		i;
	t_mlx_img	img;

	img.addr = mlx_get_data_addr(img_mlx, &img.bpp, &img.line_len, &img.endian);
	i = img.bpp -8;
	// ft_printf("i : %d\n", i);
	pixel = img.addr + (y * img.line_len + x * (img.bpp / 8));
	while (i >= 0)
	{
		// ft_printf("pixel put in : %d, %d with color %d\n", x, y, color);
		if (img.endian != 0)
			*pixel++ = (color >> i) & 0xFF;
		else
			*pixel++ = (color >> (img.bpp - 8 - i)) & 0xFF;
		i -= 8;
	}
}

static void	draw_line_bis(int *line, int *start_x, int *start_y)
{
	line[5] = line[4];
	if (line[5] > -line[0])
	{
		line[4] -= line[1];
		*start_x += line[2];
	}
	if (line[5] < line[1])
	{
		line[4] += line[0];
		*start_y += line[3];
	}
}

//line[6] from 0 to 6 is : difference_x difference_y sign_x sign_y
// check_next_pixel_pos check_next_pixel_pos 2
static void	draw_line(t_data *data, int start_x, int start_y, int end_x, int end_y, int color)
{
	// ft_printf("ENTER DRAW LINE");
	int	save_y;
	int	save_x;
	int	line[6];

	save_y = start_y;
	save_x = start_x;
	line[0] = abs(end_x - save_x);
	if (save_x < end_x)
		line[2] = 1;
	else
		line[2] = -1;
	line[1] = abs(end_y - save_y);
	if (save_y < end_y)
		line[3] = 1;
	else
		line[3] = -1;
	if (line[0] > line[1])
		line[4] = line[0] / 2;
	else
		line[4] = -line[1] / 2;
	while (save_y != end_y || save_x != end_x)
	{
		if (save_x < WIDTH_W && save_y < HEIGHT_W  && save_x >= 0 && save_y >= 0)
			img_pix_put(data->img, save_x, save_y, color);
		draw_line_bis(line, &save_x, &save_y);
	}
	// ft_printf("EXIT DRAW LINE");
}

// static int	img_pix_get(void *img_mlx, int x, int y)
// {
// 	char		*pixel;
// 	t_mlx_img	img;
// 	int			result;

// 	img.addr = mlx_get_data_addr(img_mlx, &img.bpp, &img.line_len, &img.endian);
// 	pixel = img.addr + (y * img.line_len + x * (img.bpp / 8));
// 	result = *(int *)pixel;
// 	return (result);
// }

// static int	create_trgb(int r, int g, int b)
// {
// 	return (0xFF << 24 | r << 16 | g << 8 | b);
// }

static void	draw_background(t_data *data)
{
	int			x;
	int			y;
	// ft_printf("ENTER DRAW BACK");


	y = 0;
	while (y < HEIGHT_W)
	{
		x = 0;
		while (x < WIDTH_W)
		{
			img_pix_put(data->img, x, y, WHITE);
			x++;
		}
		y++;
	}
	// ft_printf("EXIT DRAW BACK");

}

static int	draw_squarre(t_data *data, int x, int y, int color, int len)
{
	int			pos_x = x;
	int			pos_y = y;
	// ft_printf("ENTER DRAW ROOM");

	while (pos_x < x + len )
	{
		pos_y = y;

		while (pos_y < y + len)
		{
			if (pos_x < WIDTH_W && pos_y < HEIGHT_W && pos_x >= 0 && pos_y >= 0)
				img_pix_put(data->img, pos_x, pos_y, color);
			pos_y++;
		}
		pos_x++;
	}
	// ft_printf("EXIT DRAW ROOM");
	return (0);
}

static void	draw_rooms(t_data *data)
{
	int		i = 0;
	t_room	*room;
	const int mult = (LEN_OBJECT + (BORDER * 2));

	while (data->roomList && data->roomList[i] != NULL)
	{
		room = data->roomList[i];
		if (room->x * mult + BORDER >= data->cam_x - (WIDTH_W / 2) - (BORDER + LEN_OBJECT)
			&& room->x  * mult + BORDER < data->cam_x + (WIDTH_W / 2) + (BORDER + LEN_OBJECT)
			&& room->y  * mult + BORDER >= data->cam_y - (HEIGHT_W / 2) - (BORDER + LEN_OBJECT)
			&& room->y  * mult + BORDER < data->cam_y + (HEIGHT_W / 2) + (BORDER + LEN_OBJECT))
		{
			if (room->isStart)
				draw_squarre(data,
						room->x * mult + BORDER - (data->cam_x - (WIDTH_W / 2)),
						room->y * mult + BORDER - (data->cam_y - (HEIGHT_W / 2)),
						START_ROOM_COLOR, LEN_OBJECT);
			else if (room->isEnd)
				draw_squarre(data,
						room->x * mult + BORDER - (data->cam_x - (WIDTH_W / 2)),
						room->y * mult + BORDER - (data->cam_y - (HEIGHT_W / 2)),
						EXIT_ROOM_COLOR, LEN_OBJECT);
			else
				draw_squarre(data,
						room->x * mult + BORDER - (data->cam_x - (WIDTH_W / 2)),
						room->y * mult + BORDER - (data->cam_y - (HEIGHT_W / 2)),
						ROOM_COLOR, LEN_OBJECT);
		}
		i++;
	}
}

static void	draw_paths(t_data *data)
{
	int		i = 0;
	int		j = 0;
	t_room	*room;
	const int mult = (LEN_OBJECT + (BORDER * 2));
	const int add = (BORDER + (LEN_OBJECT / 2));

	while (data->roomList && data->roomList[i] != NULL)
	{
		room = data->roomList[i];
		if (room->x * mult + BORDER >= data->cam_x - (WIDTH_W / 2) - (BORDER + LEN_OBJECT)
			&& room->x  * mult + BORDER < data->cam_x + (WIDTH_W / 2) + (BORDER + LEN_OBJECT)
			&& room->y  * mult + BORDER >= data->cam_y - (HEIGHT_W / 2) - (BORDER + LEN_OBJECT)
			&& room->y  * mult + BORDER < data->cam_y + (HEIGHT_W / 2) + (BORDER + LEN_OBJECT))
		{
			j = 0;
			while (room->pathway && room->pathway[j])
			{
				draw_line(data, room->x * mult + add - (data->cam_x - (WIDTH_W / 2)),
								room->y * mult + add - (data->cam_y - (HEIGHT_W / 2)),
								room->pathway[j]->x * mult + add - (data->cam_x - (WIDTH_W / 2)),
								room->pathway[j]->y * mult + add - (data->cam_y - (HEIGHT_W / 2)), PATH_COLOR);
				j++;
			}
		}
		i++;
	}
}

static void	draw_ants(t_data *data)
{
	int		i = 0;
	t_ant	*actuAnt;
	int		r1x;
	int		r2x;
	int		r1y;
	int		r2y;
	const int mult = (LEN_OBJECT + (BORDER * 2));
	const int add = (BORDER + (LEN_OBJECT / 4));

	while (data->AMIset && data->AMIset[data->stepActual] && (data->AMIset[data->stepActual][i].actual || data->AMIset[data->stepActual][i].toGo))
	{
		actuAnt = &data->AMIset[data->stepActual][i];
		if (actuAnt->actual && actuAnt->toGo && data->stepAdvancement)
		{
			r1x = actuAnt->actual->x * mult + add - (data->cam_x - (WIDTH_W / 2));
			r1y = actuAnt->actual->y * mult + add - (data->cam_y - (HEIGHT_W / 2));
			r2x = actuAnt->toGo->x * mult + add - (data->cam_x - (WIDTH_W / 2));
			r2y = actuAnt->toGo->y * mult + add - (data->cam_y - (HEIGHT_W / 2));
			draw_squarre(data,
						((r2x - r1x) * data->stepAdvancement )/ 100 + r1x,
						((r2y - r1y) * data->stepAdvancement) / 100 + r1y,
						ANT_COLOR, LEN_ANT);
		}
		else
		{
			if (actuAnt->actual)
			{
					draw_squarre(data,
						actuAnt->actual->x * mult + add - (data->cam_x - (WIDTH_W / 2)),
						actuAnt->actual->y * mult + add - (data->cam_y - (HEIGHT_W / 2)),
						ANT_COLOR, LEN_ANT);
			}
		}
		i++;
	}
}

static void	draw_room_name(t_data *data)
{
	int		i = 0;
	t_room	*room;
	const int mult = (LEN_OBJECT + (BORDER * 2));

	while (data->roomList && data->roomList[i] != NULL)
	{
		room = data->roomList[i];
		if (room->x * mult + BORDER >= data->cam_x - (WIDTH_W / 2) - (BORDER + LEN_OBJECT)
			&& room->x  * mult + BORDER < data->cam_x + (WIDTH_W / 2) + (BORDER + LEN_OBJECT)
			&& room->y  * mult + BORDER >= data->cam_y - (HEIGHT_W / 2) - (BORDER + LEN_OBJECT)
			&& room->y  * mult + BORDER < data->cam_y + (HEIGHT_W / 2) + (BORDER + LEN_OBJECT))
		{
			if (!data->isPannelOn ||
				(room->x * mult + BORDER - (data->cam_x - (WIDTH_W / 2)) <  WIDTH_W - 190 || room->y * mult + BORDER - (data->cam_y - (HEIGHT_W / 2)) < HEIGHT_W - 190))
			{
				mlx_string_put(data->id_mlx ,data->window,
					room->x * mult + BORDER - (data->cam_x - (WIDTH_W / 2)),
					room->y * mult + BORDER - (data->cam_y - (HEIGHT_W / 2)),
					ROOM_NAME_COLOR,
					room->room);
			}
		}
		i++;
	}
}

static void draw_text_pannel_info(t_data *data)
{
	char	*str = NULL;
	mlx_string_put(data->id_mlx ,data->window, WIDTH_W - 180, HEIGHT_W - 160, PANNEL_TEXT_COLOR, "number of ants for :");
	mlx_string_put(data->id_mlx ,data->window, WIDTH_W - 180, HEIGHT_W - 140, PANNEL_TEXT_COLOR, "- start :");
	mlx_string_put(data->id_mlx ,data->window, WIDTH_W - 180, HEIGHT_W - 120, PANNEL_TEXT_COLOR, "- end   :");
	str = ft_itoa(data->start->ants);
	if (str)
	{
		mlx_string_put(data->id_mlx ,data->window, WIDTH_W - 120, HEIGHT_W - 140, PANNEL_TEXT_COLOR, str);
		free(str);
	}
	else
		mlx_string_put(data->id_mlx ,data->window, WIDTH_W - 120, HEIGHT_W - 140, PANNEL_TEXT_COLOR, " ??? ");
	str = ft_itoa(data->end->ants);
	if (str)
	{
		mlx_string_put(data->id_mlx ,data->window, WIDTH_W - 120, HEIGHT_W - 120, PANNEL_TEXT_COLOR, str);
		free(str);
	}
	else
		mlx_string_put(data->id_mlx ,data->window, WIDTH_W - 120, HEIGHT_W - 120, PANNEL_TEXT_COLOR, " ??? ");
	mlx_string_put(data->id_mlx ,data->window, WIDTH_W - 180, HEIGHT_W - 80, PANNEL_TEXT_COLOR, "number total of ants :");
	str = ft_itoa(data->total_ants);
	if (str)
	{
		mlx_string_put(data->id_mlx ,data->window, WIDTH_W - 40, HEIGHT_W - 80, PANNEL_TEXT_COLOR, str);
		free(str);
	}
	else
		mlx_string_put(data->id_mlx ,data->window, WIDTH_W - 40, HEIGHT_W - 80, PANNEL_TEXT_COLOR, " ??? ");
}

void	draw_ants_colony(t_data *data)
{
	draw_background(data);
	draw_paths(data);
	draw_rooms(data);
	draw_ants(data);
	if (data->isPannelOn)
	{
		draw_squarre(data, WIDTH_W - 190, HEIGHT_W - 190, PANNEL_BACKGROUND_COLOR, 190);
		draw_line(data, WIDTH_W - 190, HEIGHT_W - 190, WIDTH_W, HEIGHT_W - 190, PANNEL_BORDER_COLOR);
		draw_line(data, WIDTH_W - 190, HEIGHT_W - 189, WIDTH_W, HEIGHT_W - 189, PANNEL_BORDER_COLOR);
		draw_line(data, WIDTH_W - 190, HEIGHT_W - 190, WIDTH_W - 190, HEIGHT_W, PANNEL_BORDER_COLOR);
		draw_line(data, WIDTH_W - 189, HEIGHT_W - 190, WIDTH_W - 189, HEIGHT_W, PANNEL_BORDER_COLOR);

	}
	mlx_put_image_to_window(data->id_mlx, data->window, data->img, 0, 0);
	if (data->isPannelOn)
		draw_text_pannel_info(data);
	draw_room_name(data);
}
