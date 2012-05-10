
export aroma = love if love

import rectangle, setColor, getColor from aroma.graphics
import insert from table

class Piece
  rot: {2, 2} -- rotation origin

  new: (@board, @ox=0, @oy=0) =>
    error "need shape" if not @shape
    @pts = {}
    for y=1,#@shape
      for x=1,#@shape[y]
        if @shape[y][x] > 0
          insert @pts, {x, y}

  -- clockwise unless flipped
  rotate: (flip=false)=>
    rot_x, rot_y = unpack @rot
    @pts = for pt in *@pts
      x = pt[1] - rot_x
      y = pt[2] - rot_y

      x,y = if flip
        y, -x
      else
        -y, x

      {x + rot_x, y + rot_y}


  each_pt: =>
    coroutine.wrap ->
      for pt in *@pts
        x, y = pt[1] + @ox, pt[2] + @oy
        coroutine.yield x,y

  try_rotate: =>
    @rotate!
    @rotate true if @collides!

  try_move: (dx, dy) =>
    @ox += dx
    @oy += dy
    if @collides!
      @ox -= dx
      @oy -= dy
      false
    else
      true

  collides: =>
    for x, y in @each_pt!
      return true if x < 1 or x > @board.width or y < 1 or y > @board.height
      return true if @board.grid[x][y]

    false

  draw: =>
    setColor @color
    for x, y in @each_pt!
      @board\draw_cell x, y

class Ell extends Piece
  rot: {1, 2}
  color:  {250, 94, 20}
  shape: {
    {1,1}
    {1,0}
    {1,0}
  }


class Seven extends Piece
  color:  {229, 59, 116}
  shape: {
    {1,1}
    {0,1}
    {0,1}
  }


class Tee extends Piece
  color:  {196, 229, 59}
  shape: {
    {0,1,0}
    {1,1,1}
  }


class Ess extends Piece
  color:  {59, 229, 122}
  shape: {
    {0,1,1}
    {1,1,0}
  }

class Zee extends Piece
  color:  {229, 83, 59}
  shape: {
    {1,1,0}
    {0,1,1}
  }

class Oo extends Piece
  color:  {229, 219, 59}
  shape: {
    {1,1}
    {1,1}
  }


class Ii extends Piece
  rot: {1, 2}
  color:  {59, 107, 229}
  shape: {
    {1}
    {1}
    {1}
    {1}
  }

class Board
  cell_size: 14
  padding: 1

  ox: 40
  oy: 40

  color: {200, 200, 200}

  new: (@width, @height) =>
    @grid = {}
    for x=1,@width
      col = {}
      for y=1,@height
        col[y] = false
      @grid[x] = col
    
    @center!

  center: =>
    -- center in window
    size = @cell_size + @padding
    mx, my = @width * size + 2, @height * size + 2

    @ox = (aroma.graphics.getWidth! - mx) / 2
    @oy = (aroma.graphics.getHeight! - my) / 2

  set_piece: (piece) =>
    to_check = {}
    for x,y in piece\each_pt!
      to_check[y] = true
      @set x,y

    for y in pairs to_check
      print "checking", y
      @check_row y


  -- check if a row is completed
  check_row: (y) =>
    for i=1,@width
      return if not @grid[i][y]

    print "got row"

    for yy=y,1,-1
      for xx=1,@width
        if yy < 1
          @grid[xx][yy] = false
        else
          @grid[xx][yy] = @grid[xx][yy - 1]

  set: (x,y) =>
    @grid[x][y] = true

  draw_cell: (x, y, color) =>
    setColor color if color
    size = @cell_size + @padding
    rectangle "fill",
      @ox + (x - 1)*size, @oy + (y - 1)*size,
      @cell_size, @cell_size

  draw: =>
    size = @cell_size + @padding

    setColor 255,255,255

    rectangle "line",
      @ox - 1, @oy - 1,
      @width * size + 2, @height * size + 2
    
    setColor @color

    for y=1,@height
      for x=1,@width
        val = @grid[x][y]
        if val then @draw_cell x, y


class Game
  pieces: {
    Ell, Seven, Tee, Ess, Zee, Oo, Ii
  }

  new: =>
    @speed = 0.2
    @board = Board 10, 20

  keypressed: (name, code) =>
    os.exit! if code == 27
    if @current_piece
      switch name
        when "left"
          @current_piece\try_move -1, 0
        when "right"
          @current_piece\try_move 1, 0
        when "down"
          @push_down!
        when "up"
          @current_piece\try_rotate!
        when " "
          while @push_down! do nil
          nil
          

  push_down: =>
    if not @current_piece\try_move 0, 1
      @board\set_piece @current_piece
      @current_piece = nil
      false
    else true

  update: (dt) =>
    if not @current_piece
      cls = @pieces[math.random 1, #@pieces]
      @current_piece = cls @board,
        cls.rot[1] + math.floor(@board.width / 3),
        cls.rot[2]

      error "game over" if @current_piece\collides!
      @last_tick = 0

    if @last_tick > @speed
      @last_tick -= @speed
      @push_down!

    @last_tick += dt

  draw: =>
    @board\draw!
    @current_piece\draw! if @current_piece

aroma.load = ->
  game = Game!
  aroma.update = game\update
  aroma.draw = game\draw
  aroma.keypressed = game\keypressed

