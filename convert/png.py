from PIL import Image, ImageDraw


class PNG:
	def __init__(self, player_number, color_table, picture_data):
		self.player_number = player_number
		self.color_table = color_table
		self.picture_data = picture_data

	def create(self, pwidth=None, pheight=None, alphamask=False):
		if pwidth == None and pheight == None:
			self.width = len(self.picture_data[0])
			self.height = len(self.picture_data)
		else:
			self.width = pwidth
			self.height = pheight

		self.image = Image.new('RGBA', (self.width, self.height), (255, 255, 255, 0))
		draw = ImageDraw.ImageDraw(self.image)

		if not alphamask:
			self.draw_picture(draw)
		else:
			self.draw_alphamask(draw)

	def draw_alphamask(self, draw):
		for y, picture_row in enumerate(self.picture_data):

			for x, alpha_data in enumerate(picture_row):
				val = 255 - alpha_data*2
				if alpha_data == -1:
					alpha = 0
				else:
					alpha = 255

				draw.point((x, y), fill=(val, val, val, alpha))

	def draw_picture(self, draw):
		#avoid circular importing..
		from slp import SLPFrame

		# TODO draw lines, more efficient picture_data
		for y, picture_row in enumerate(self.picture_data):
			for x, color_data in enumerate(picture_row):
				if type(color_data) == int:
					#simply look up the color index in the table
					color = self.color_table[color_data]
				elif type(color_data) == SLPFrame.SpecialColor:
					base_pcolor, is_outline = color_data.get_pcolor()
					if is_outline:
						alpha = 253  #mark this pixel as outline
						#dbg("drawing outline base %d" % base_pcolor)
					else:
						alpha = 254  #mark this pixel as player color

					#get rgb base color from the color table
					#store it the preview player color (in the table: [16*player,16*player+7]
					r, g, b = self.color_table[base_pcolor + 16 * self.player_number]
					color = (r, g, b, alpha)

					#this now ensures the fragment shader is able to replace the base_color
					#with the real player color, and only display outline pixels,
					#if they are obstructed.

				elif color_data == SLPFrame.transparent:
					color = (0, 0, 0, 0)
				elif color_data == SLPFrame.shadow:
					color = (0, 0, 0, 100)
				else:
					raise Exception("Unknown color: " + str(color_data))

				draw.point((x, y), fill=color)

				#self.draw.line((x, y, x + amount, y), fill=color)
