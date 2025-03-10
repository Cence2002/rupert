export class Selection {
    public selectedBox3: number | null = $state(null);
    public selectedRectangle: number | null = $state(null);

    public selectBox3(index: number) {
        this.selectedRectangle = null;
        this.selectedBox3 = index;
    }

    public selectRectangle(index: number) {
        if (this.selectedBox3 === null) {
            return;
        }
        this.selectedRectangle = index;
    }
}
