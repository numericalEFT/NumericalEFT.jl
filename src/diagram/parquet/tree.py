import ete3 as ete


def my_layout(node):
    if node.is_leaf():
        name_face = ete.AttrFace("name", fsize=8)
    else:
        name_face = ete.AttrFace("name", fsize=10)
    ete.faces.add_face_to_node(
        name_face, node, column=0, position="branch-top")
    # ete.faces.add_face_to_node(
    #     name_face, node, column=0)


def plot(t):
    ts = ete.TreeStyle()
    ts.show_leaf_name = False
    # ts.show_leaf_name = True
    ts.layout_fn = my_layout
    t.show(tree_style=ts)
